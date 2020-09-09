<?php


use expay\request_expay as request_expay;
use expay\ExpayException as ExpayException;
/**
 * Class commerceExpay
 * @package commerceExpay
 */
class commerceExpay extends request_expay {
  // Expay method array.
  protected $methods = array();
  // Expay payment array.
  protected $payment = array();
  // Commerce payment transaction object.
  protected $transaction = NULL;
  // Use debug variable.
  public $debug;
  // request_expay class handler.
  public $call_class;
  // Response after callback validation
  public $response = array();

  /**
   * Constructor.
   */
  public function __construct() {
    $host = variable_get('commerce_expay_host', 'sandbox');
    $kkey = variable_get('commerce_expay_merchant_id', '');
    $skey = variable_get('commerce_expay_merchant_sk', '');
    // Init debug variable.
    $this->debug = variable_get('commerce_expay_debug_mode', 0);
    $debug_file = file_directory_temp() . '/commerce_expay_debug.txt';
    // Ovveride parent $error_file property.
    self::$error_file = $debug_file;
    // Need to ovveride exception logger.
    $logger = new commerceExpayLogger($debug_file);
    try {
      $this->call_class = new request_expay($host, $kkey, $skey);
    }
    catch (ExpayException $e) {
      watchdog('commerce_expay', $e->getMessage(), NULL, WATCHDOG_ERROR);
    }
    if ($this->call_class) {
      $this->call_class->debug = variable_get('commerce_expay_debug_mode', 0);
    }
    else {
      $this->logError();
    }
  }

  /**
   * Log errors into drupal watchdog.
   */
  public function logError() {
    $error = $this->getinfoexpayerror();
    if (empty($error)) {
      return;
    }
    $message = t('Message - @message. Code - @code', array('@message' => $error['message'], '@code' => $error['code']));
    watchdog('commerce_expay', $message, NULL, WATCHDOG_ERROR);
  }

  /**
   * Getting expay payment methods.
   */
  public function paymentMethods() {
    $methods = array();
    if (empty($this->call_class)) {
      return $methods;
    }
    // Try to get methods.
    try {
      $response = $this->call_class->getMethods();
    }
    catch (ExpayException $e) {
      watchdog('commerce_expay', $e->getMessage(), NULL, WATCHDOG_ERROR);
      return $methods;
    }
    // If problem - return empty methods list
    // and log error.
    if (empty($response)) {
      $this->logError();
      return $methods;
    }
    // Payment methods request was correct.
    // Check payment methods list.
    if (!empty($response['methods'])) {
      foreach ($response['methods'] as $method) {
        // Type always must be filled.
        if (empty($method['type'])) {
          continue;
        }
        $methods[$method['id']] = $method;
      }
    }
    // Debug record.
    if ($this->debug) {
      watchdog('commerce_expay', 'Expay pymentMethods response: <br /> <pre>@methods</pre>', array('@methods' => print_r($methods, TRUE)), WATCHDOG_NOTICE);
    }
    // Setting class property.
    $this->methods = $methods;

    return $methods;
  }

  /**
   * Init remote Expay payment.
   */
  public function payment($params) {
    $payment = array();
    if (empty($this->call_class)) {
      return $payment;
    }
    if ($this->debug) {
      watchdog('commerce_expay', 'Expay initPayment params: <br /> <pre>@params</pre>', array('@params' => print_r($params, TRUE)), WATCHDOG_NOTICE);
    }
    // Try to init payment.
    try {
      $payment = $this->call_class->initPayment($params);
    }
    catch (ExpayException $e) {
      watchdog('commerce_expay', $e->getMessage(), NULL, WATCHDOG_ERROR);
      return $payment;
    }
    // If payment errors - log error message.
    if (empty($payment)) {
      $this->logError();
    }
    if ($this->debug) {
      watchdog('commerce_expay', 'Expay initPayment response: <br /> <pre>@payment</pre>', array('@payment' => print_r($payment, TRUE)), WATCHDOG_NOTICE);
    }
    return $payment;
  }

  /**
   * Validate callback method notification.
   */
  public function callback_method_validate($method, $post) {
    // Check if we have such method.
    if (!in_array($method, array('check', 'pay', 'status', 'cancel'))) {
      $this->response = array('status' => '401', 'message' => "Invalid requested order parameters");
      return FALSE;
    }
    // Checking income params.
    $remote_payment_id = trim($post['id']);
    $remote_payment_order = trim($post['order']);
    // Try to load commerce payment transaction by remote payment id.
    $payment_id = commerce_expay_remote_order_parse($remote_payment_order, 'transaction');
    $order_id = commerce_expay_remote_order_parse($remote_payment_order, 'order');
    $options = array(
      'transaction_id' => $payment_id,
      'order_id' => $order_id
    );
    $transactions = commerce_payment_transaction_load_multiple(array(), $options);
    // In case no transactions were not found.
    if (empty($transactions)) {
      // Log wrong notification callback.
      $log_params = array('@m' => $method, '@id' => $remote_payment_id, '@order' => $remote_payment_order);
      watchdog('commerce_expay', 'Expay callback load transaction empty with (m-@m, id-@id, order-@order) ', $log_params, WATCHDOG_NOTICE);
      $this->response = commerce_expay_callback_wrong_params_response($method);
      return FALSE;
    }
    $transaction = end($transactions);
    $this->transaction = $transaction;
    $payload = $transaction->payload;
    if (empty($payload) || empty($payload['expay_payment'])) {
      $log_params = array('@m' => $method, '@transaction_id' => $transaction->transaction_id);
      watchdog('commerce_expay', 'Expay callback @m transaction @transaction_id payload expay payment empty.', $log_params, WATCHDOG_NOTICE);
      $this->response = commerce_expay_callback_wrong_params_response($method);
      return FALSE;
    }
    $expay_payment = $payload['expay_payment'];
    if (empty($expay_payment['order']) || drupal_strtolower($expay_payment['order']) != drupal_strtolower($remote_payment_order)) {
      $log_params = array('@m' => $method, '@transaction_id' => $transaction->transaction_id, '@order' => $remote_payment_order);
      watchdog('commerce_expay', 'Expay callback @m transaction @transaction_id payload order != @order .', $log_params, WATCHDOG_NOTICE);
      $this->response = commerce_expay_callback_wrong_params_response($method);
      return FALSE;
    }
    if (empty($expay_payment['id']) || $expay_payment['id'] != $remote_payment_id) {
      $log_params = array('@m' => $method, '@transaction_id' => $transaction->transaction_id, '@id' => $remote_payment_id);
      watchdog('commerce_expay', 'Expay callback @m transaction @transaction_id payload id != @id .', $log_params, WATCHDOG_NOTICE);
      $this->response = commerce_expay_callback_wrong_params_response($method);
      return FALSE;
    }
    $callback_expire = variable_get('commerce_expay_callback_expire', 86400);
    if (!((time() - $callback_expire) < $post['timestamp']) || !((time() + $callback_expire) > $post['timestamp'])) {
      $log_params = array('@m' => $method, '@transaction_id' => $transaction->transaction_id, '@time' => $post['timestamp']);
      watchdog('commerce_expay', 'Expay callback @m transaction @transaction_id expired @time .', $log_params, WATCHDOG_NOTICE);
      $this->response = commerce_expay_callback_wrong_params_response($method);
      return FALSE;
    }
    // Stop checking for status.
    // There are no service_id and oamount in status callback.
    if ($method == 'status') {
      return TRUE;
    }
    $expay_method = $payload['expay_method'];
    $service_id = $post['service_id'];
    if (empty($expay_method['id']) || (string)$expay_method['id'] != (string)$service_id) {
      $log_params = array('@m' => $method ,'@transaction_id' => $transaction->transaction_id, '@service_id' => $service_id);
      watchdog('commerce_expay', 'Expay callback @m transaction @transaction_id payload method id != @service_id.', $log_params, WATCHDOG_NOTICE);
      $this->response = commerce_expay_callback_wrong_params_response($method);
      return FALSE;
    }
    $amount = commerce_currency_decimal_to_amount(trim($post['amount']), $transaction->currency_code);
    if ($transaction->amount != $amount) {
      $log_params = array('@m' => $method, '@transaction_id' => $transaction->transaction_id, '@amount' => $amount);
      watchdog('commerce_expay', 'Expay callback @m transaction @transaction_id amount != @amount .', $log_params, WATCHDOG_NOTICE);
      $this->response = commerce_expay_callback_wrong_params_response($method);
      return FALSE;
    }
    return TRUE;
  }

  /**
   * Return current loaded transaction during callback request.
   */
  public function callback_method_transaction() {
    return $this->transaction;
  }
}

// Helper class to ovveride expay\Logger error file.
class commerceExpayLogger extends expay\Logger {
  public function __construct($file) {
    self::$error_file = $file;
  }
}

/**
 * Wrapper function to get expay methods list.
 *
 * @return array
 */
function commerce_expay_get_methods($reset = FALSE, $flush_logos = FALSE) {
  $methods = &drupal_static(__FUNCTION__);
  if (!$reset && isset($methods)) {
    return $methods;
  }
  else {
    if (!$reset && $cache = cache_get('commerce_expay_methods')) {
      $methods = $cache->data;
    }
    else {
      $handler = new commerceExpay();
      $methods = $handler->paymentMethods();
      cache_set('commerce_expay_methods', $methods, 'cache');
    }
    if ($flush_logos) {
      foreach ($methods as $method) {
        $local_uri = commerce_expay_fetch_logos($method);
      }
    }
    return $methods;
  }
  return NULL;
}

/**
 * Init Expay payment gateway.
 *
 * @return commerceExpay handler or FALSE.
 */
function commerce_expay_getaway() {
  $handler = &drupal_static(__FUNCTION__);
  if (isset($handler)) {
    return $handler;
  }
  else {
    try {
      $handler = new commerceExpay();
    }
    catch (ExpayException $e) {
       watchdog('commerce_expay', $e->getMessage(), NULL, WATCHDOG_ERROR);
       return NULL;
    }
    return $handler;
  }
  return NULL;
}

/**
 * Update commerce transaction according server response.
 *
 * @param  object $transaction
 *   Commerce transaction object.
 * @param  string $remote_id
 *   Transaction remote ID.
 * @return boolen TRUE or FALSE.
 */
function commerce_expay_transaction_status_update($transaction, $remote_order_id, $force_save = FALSE) {
  // Update only pending transactions.
  if (in_array($transaction->status, array(COMMERCE_PAYMENT_STATUS_FAILURE))) {
    return FALSE;
  }
  try {
    $handler = commerce_expay_getaway();
    $remote_status = $handler->call_class->getStatus(array('order' => $remote_order_id));
  }
  catch (ExpayException $e) {
    watchdog('commerce_expay', $e->getMessage(), NULL, WATCHDOG_ERROR);
    return FALSE;
  }
  if (empty($remote_status)) {
    $handler->logError();
    watchdog('commerce_expay', 'Expay getStatus returned empty object', NULL, WATCHDOG_ERROR);
    return FALSE;
  }
  $debug = variable_get('commerce_expay_debug_mode', 0);
  if ($debug) {
    watchdog('commerce_expay', 'Request getStatus: <br /> <pre>@remote</pre>', array('@remote' => print_r($remote_status, TRUE)), WATCHDOG_NOTICE);
  }
  // Getting transaction order.
  $order = commerce_order_load($transaction->order_id);
  $transaction->message = t('Transaction updated on getStatus with status @status', array('@status' => $remote_status['status']));
  $transaction->payload['expay_updates'][] = array(
    'type' => 'getstatus',
    'data' => $remote_status
  );
  switch ($remote_status['status']) {
    case EXPAY_GETSTATUS_PROCESSING_QUEUE:
    case EXPAY_GETSTATUS_PROCESSING:
    case EXPAY_GETSTATUS_WAITING_PAYER:
      $transaction->status = COMMERCE_PAYMENT_STATUS_PENDING;
      break;
    case EXPAY_GETSTATUS_SUCCESS:
      $transaction->status = COMMERCE_PAYMENT_STATUS_SUCCESS;
      break;
    case EXPAY_GETSTATUS_REJECTED:
    case EXPAY_GETSTATUS_RETURNED:
      $transaction->status = COMMERCE_PAYMENT_STATUS_FAILURE;
      break;
  }
  if ($force_save) {
    commerce_payment_transaction_save($transaction);
  }
  $expay_method = $transaction->payload['expay_method'];
  $expay_method_type = drupal_strtolower($expay_method['type']);
  rules_invoke_all('commerce_expay_transaction_status_update', $order, $transaction, trim($expay_method_type), $remote_status['status']);

  return TRUE;
}

/**
 * Validate payment notification request IP.
 *
 * @return  TRUE or FALSE
 */
function commerce_expay_callback_check_ip() {
  $ip = ip_address();
  // Do not validate if IP list is empty.
  $valid_ips_string = trim(variable_get('commerce_expay_allowed_ips', ''));
  if (empty($valid_ips_string)) {
    return TRUE;
  }
  // Check valid IPs.
  $valid_ips = explode(',', $valid_ips_string);
  $ips = array_map("trim", $valid_ips);
  return in_array($ip, $ips);
}

/**
 * Helper function to get response on wrong payment params message.
 *
 * @return  array response
 */
function commerce_expay_callback_wrong_params_response($method) {
  $error_message = "Payment with the specified parameters was not found";
  switch ($method) {
    case 'check':
      $response = array('status' => 475, 'message' => $error_message);
      break;
    case 'pay':
    case 'status':
      $response = array('status' => 474, 'message' => $error_message);
      break;
    default:
      break;
  }
  return $response;
}

/**
 * Check method response.
 *
 * @param  object $transaction
 *   Commerce transaction object.
 * @return array
 *   Response object.
 */
function commerce_expay_check_response($transaction) {
  $response = array();
  switch ($transaction->status) {
    case COMMERCE_PAYMENT_STATUS_SUCCESS:
      $response = array('status' => 475, 'message' => COMMERCE_PAYMENT_STATUS_SUCCESS);
      break;
    case COMMERCE_PAYMENT_STATUS_PENDING:
      $response = array('status' => 270, 'message' => COMMERCE_PAYMENT_STATUS_PENDING);
      break;
    case COMMERCE_PAYMENT_STATUS_FAILURE:
      $response = array('status' => 475, 'message' => COMMERCE_PAYMENT_STATUS_FAILURE);
      break;
  }
  return $response;
}

/**
 * Pay method response.
 *
 * @param  object $transaction
 *   Commerce transaction object.
 * @return array
 *   Response object.
 */
function commerce_expay_pay_response($transaction) {

  $response = array();
  switch ($transaction->status) {
    case COMMERCE_PAYMENT_STATUS_SUCCESS:
      $response = array('status' => 205, 'message' => COMMERCE_PAYMENT_STATUS_SUCCESS);
      break;
    case COMMERCE_PAYMENT_STATUS_PENDING:
      $response = array('status' => 206, 'message' => COMMERCE_PAYMENT_STATUS_PENDING);
      break;
    case COMMERCE_PAYMENT_STATUS_FAILURE:
      $response = array('status' => 204, 'message' => COMMERCE_PAYMENT_STATUS_FAILURE);
      break;
  }
  return $response;
}

/**
 * Status method response.
 *
 * @param  object $transaction
 *   Commerce transaction object.
 * @return array
 *   Response object.
 */
function commerce_expay_status_response($transaction) {
  $response = array();
  switch ($transaction->status) {
    case COMMERCE_PAYMENT_STATUS_SUCCESS:
      $response = array('status' => 205, 'message' => COMMERCE_PAYMENT_STATUS_SUCCESS);
      break;
    case COMMERCE_PAYMENT_STATUS_PENDING:
      $response = array('status' => 206, 'message' => COMMERCE_PAYMENT_STATUS_PENDING);
      break;
    case COMMERCE_PAYMENT_STATUS_FAILURE:
      $response = array('status' => 204, 'message' => COMMERCE_PAYMENT_STATUS_FAILURE);
      break;
  }
  return $response;
}

/**
 * Helper function to save external payment method logo.
 *
 * @return Logo image url. FALSE if error.
 */
function commerce_expay_fetch_logos($method) {
  if (empty($method['image'])) {
    return FALSE;
  }
  $url = $method['image'];
  $result = drupal_http_request($url);
  $code = floor($result->code / 100) * 100;
  if ($code != 200) {
    return FALSE;
  }
  $default_scheme = file_default_scheme();
  $directory = $default_scheme . '://' . variable_get('commerce_expay_logos_directory', 'commerce_expay_logos');
  $filename = 'payment_method_' . $method['id'];
  // Add the extension for real images.
  if ($extension = pathinfo($url, PATHINFO_EXTENSION)) {
    if (in_array($extension, array('jpg', 'png', 'gif'))) {
      $filename .= '.' . $extension;
    }
    else {
      watchdog('commerce_expay', 'Wrong extension @extension for @url', array('@extension' => $extension, '@url' => $url), WATCHDOG_ERROR);
    }
  }
  if (file_prepare_directory($directory, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS)) {
    $file_path = $directory . '/' . $filename;
    return file_unmanaged_save_data($result->data, $file_path, FILE_EXISTS_REPLACE);
  }
  return FALSE;

}

/**
 * Helper function to get payment method logo url.
 *
 * @return Logo image url. FALSE if error.
 */
function commerce_expay_method_logo_url($method) {
  if (empty($method['image'])) {
    return FALSE;
  }
  $local_logo_only = variable_get('commerce_expay_logos_local_only', TRUE);
  if (!$local_logo_only) {
    $check = commerce_expay_method_logo_check($method['image']);
    if (!$check) {
      return FALSE;
    }
    return $method['image'];
  }
  $default_scheme = file_default_scheme();
  $directory = $default_scheme . '://' . variable_get('commerce_expay_logos_directory', 'commerce_expay_logos');
  $filename = $directory . '/payment_method_' . $method['id'];
  if ($extension = pathinfo($method['image'], PATHINFO_EXTENSION)) {
    if (in_array($extension, array('jpg', 'png', 'gif'))) {
      $filename .= '.' . $extension;
    }
  }
  if (file_exists($filename)) {
    return $filename;
  }
  return FALSE;
}

/**
 * Checks if remove logo accessible .
 *
 * @return FALSE or TRUE.
 */
function commerce_expay_method_logo_check($url) {
  $ch = curl_init();
  curl_setopt($ch, CURLOPT_HEADER, TRUE);
  curl_setopt($ch, CURLOPT_NOBODY, TRUE);
  curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, 0);
  curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
  curl_setopt($ch, CURLOPT_URL, $url);
  $out = curl_exec($ch);
  $status = curl_getinfo($ch, CURLINFO_HTTP_CODE);
  curl_close($ch);
  return ($status == 200) ? TRUE : FALSE;
}

/**
 * Helper function to get Expay method id from
 * commerce payment method
 *
 * @param  array $payment_method
 *    Commerce payment method info.
 * @return int
 *    Expay payment method id.
 */
function _commerce_expay_payment_method_id($payment_method) {
  $expay_method_id = substr($payment_method['method_id'], 13);
  return $expay_method_id;
}

/**
 * Helper function to generate Expay order param
 * based on commerce order ID and commerce transaction ID.
 */
function _commerce_expay_transaction_id($order_id, $transaction_id) {
  $pattern = variable_get('commerce_expay_remote_id_pattern', 'ORDER!oidTR!tid');
  $out = '';
  $prefix = variable_get('commerce_expay_remote_id_prefix', '');
  if (!empty($prefix)) {
    $out .= $prefix;
  }
  $out .= format_string($pattern, array('!oid' => $order_id, '!tid' => $transaction_id));
  return $out;
}

/**
 * Helper function to get commerce order ID or commerce transaction ID
 * form Expay order param.
 */
function commerce_expay_remote_order_parse($remote_order, $type = 'order') {
  $types = array('order', 'transaction');
  if (!in_array($type, $types)) {
    return FALSE;
  }
  $pattern = variable_get('commerce_expay_remote_id_pattern', 'ORDER!oidTR!tid');
  if ($type == 'order') {
    $pattern_preg = str_replace(array('!tid', '!oid'), array('[0-9]+', '([0-9]+)'), $pattern);
  }
  if ($type == 'transaction') {
    $pattern_preg = str_replace(array('!tid', '!oid'), array('([0-9]+)', '[0-9]+'), $pattern);
  }
  preg_match('/'. $pattern_preg . '/', $remote_order, $matches);
  if (empty($matches[1])) {
    return FALSE;
  }
  return $matches[1];
}

/**
 * Getting remote payment return datas.
 * Can be redirect url, image or external pdf url.
 *
 * @param  array remote_payment
 *    Expay method data.
 * @return array
 *    Array with data type and value.
 */
function commerce_expay_payment_data($remote_payment) {
  $attributes = $remote_payment['attributes'];
  $data = NULL;
  foreach ($attributes as $attribute) {
    // Redirect url.
    if ($attribute['key'] == 'redirectUrl') {
      $data['value'] = $attribute['value'];
      $data['description'] = '';
      $data['type'] = 'redirect';
    }
    if ($attribute['key'] == 'showData') {
      // By default consider QR code image.
      $data['value'] = base64_decode($attribute['value']);
      $data['description'] = $attribute['description'];
      $data['type'] = 'image';
      // Pdf file link.
      if (preg_match('/<a href="(.+)">.+<\/a>/', $data['value'], $match)) {
        $data['value'] = $match[0];
        $data['type'] = 'file';
      }
    }
  }
  return $data;
}

/**
 * Generate additional provider fields.
 *
 * @param  array $provider
 *    Expay method.
 * @return array
 *    Drupal form elements.
 */
function commerce_expay_provider_fields($provider) {
  $fields = array();
  if (empty($provider)) {
    return $fields;
  }
  $attributes = $provider['attributes'];

  if (empty($attributes)) {
    return $fields;
  }
  foreach ($attributes as $key => $field) {
    $field_id = $field['key'];
    $fields[$field_id] = array(
      '#type' => 'textfield',
      '#title' => $field['name'],
      '#description' => $field['description'],
      '#required' => ((bool)$field['required']) ? TRUE : FALSE
    );
  }
  return $fields;
}

/**
 * Helper function to cancel transaction.
 */
function commerce_expay_cancel_transaction($transaction, $message) {
  $transaction->status = COMMERCE_PAYMENT_STATUS_FAILURE;
  $transaction->payload['expay_updates'][] = array(
    'type' => 'error',
    'data' => $message
  );
  commerce_payment_transaction_save($transaction);
}
