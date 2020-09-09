<?php

/**
 * @file
 * Checkout pane callback functions for the expay module.
 */


/**
 * Checkout pane callback: displays showData content for affline methods
 * on the complete page.
 */
function commerce_expay_complete_pane_checkout_form($form, &$form_state, $checkout_pane, $order) {
  $pane_form = array();
  // Perams to filter valid transactions.
  $options = array(
    'order_id' => $order->order_id,
    'status' => COMMERCE_PAYMENT_STATUS_PENDING,
    'payment_method' => 'commerce_expay'
  );
  $transactions = commerce_payment_transaction_load_multiple(array(), $options);
  if (count($transactions) > 0) {
    // Use last transaction.
    $transaction = end($transactions);
  }
  if (!empty($transaction->payload['expay_payment'])) {
    $remote_payment = $transaction->payload['expay_payment'];
  }
  // Getting remote payment display data.
  $remote_data = commerce_expay_payment_data($remote_payment);
  // In case we have QR code or PDF direct link.
  $pane_form['remote_data'] = array(
    '#type' => 'container',
    '#weight' => 18
  );
  // Remote data description.
  if (!empty($remote_data['description'])) {
    $pane_form['remote_data']['remote_data_description'] = array(
      '#markup' => '<p>' . check_plain($remote_data['description']) . '</p>',
    );
  }
  // Remote data pane message.
  $pane_form['remote_data']['remote_document_description'] = array(
    '#markup' => '<h4>' . t('Attention! Save the document to pay before closing the page!') . '</h4>',
  );
  // Remote data has PDF file or QR code image direct link.
  $pane_form['remote_data']['remote_data_value'] = array(
    '#type' => 'item',
    '#markup' => $remote_data['value'],
    '#attributes' => array('class' => array('expay-remote-data-value'))
  );

  $pane_form['view_order_controles'] = array(
    '#type' => 'container',
    '#weight' => 19,
    '#attributes' => array('class' => array('expay-complete-links'))
  );
  $view_url = 'user/' . $order->uid . '/orders/' . $order->order_id;
  $pane_form['view_order_controles']['view_order_link'] = array(
    '#markup' => l(t('View order'), $view_url, array('absolute' => TRUE)),
  );
  $pane_form['cancel_order'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel and return'),
    '#submit' => array('commerce_expay_complete_pane_submit_cancel_order'),
    '#weight' => 20
  );
  $pane_form['front_redirect_link'] = array(
    '#type' => 'container',
    '#weight' => 21,
    '#attributes' => array('class' => array('expay-complete-links'))
  );
  $pane_form['front_redirect_link']['return_home_link'] = array(
    '#markup' => l(t('Continue buying'), '<front>', array('absolute' => TRUE)),
  );
  $pane_form['#attached']['css'][] = drupal_get_path('module', 'commerce_expay') . '/theme/commerce_expay.theme.css';

  return $pane_form;
}

/**
 * Helper submit to proceed customer with redirect in user zone order view.
 */
function commerce_expay_complete_pane_submit_view_order($form, &$form_state) {
  $order = $form_state['order'];
  $redirect_url = url('user/' . $order->uid . '/orders/' . $order->order_id, array('absolute' => TRUE));
  drupal_goto($redirect_url);
}

/**
 * Helper submit to cancel order.
 */
function commerce_expay_complete_pane_submit_cancel_order($form, &$form_state) {
  $order = commerce_order_load($form_state['order']->order_id);
  $form_state['order'] = commerce_order_status_update($order, 'canceled', TRUE);
  $form_state['order']->revision = TRUE;
  $form_state['order']->log = t('Customer canceled order on checkout complete.');
  commerce_order_save($form_state['order']);
  // Cancel transaction.
  $options = array(
    'order_id' => $order->order_id,
    'status' => COMMERCE_PAYMENT_STATUS_PENDING,
    'payment_method' => 'commerce_expay'
  );
  $transactions = commerce_payment_transaction_load_multiple(array(), $options);
  if (count($transactions) > 0) {
    // Use last transaction.
    $transaction = end($transactions);
    $transaction->status = COMMERCE_PAYMENT_STATUS_FAILURE;
    $transaction->message = t('Transaction canceled on order complete.');
    commerce_payment_transaction_save($transaction);
  }
  // Redirect to cart on cancel.
  $form_state['redirect'] = '<front>';
}
