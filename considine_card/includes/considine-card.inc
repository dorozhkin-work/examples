<?php

/**
 * @file
 * Defines additional function for Considine card functionality.
 */


/**
 * Function to create default card for current user.
 */
function considine_card_card_new($uid = 0) {
  global $user;

  // Create new card object.
  $new_card = array(
    'entityform_id' => 0,
    'type' => CONSIDINE_CARD_TYPE,
    'uid' => $uid,
    'created' => REQUEST_TIME,
    'changed' => REQUEST_TIME
  );

  $card = entityform_create($new_card);

  considine_card_card_populate($card);

  entityform_save($card);

  if (!$uid && empty($user->uid)) {
    considine_card_session_save($card->entityform_id);
  }

  return $card;
}

/**
 * Function to populate card object with default values.
 */
function considine_card_card_populate($card) {
  $card_wrapper = entity_metadata_wrapper('entityform', $card);

  // Card product.
  // TODO: delete product adding during populate.
  // Product must be added before populate.
  $product_id = variable_get('considine_card_product', 3);
  $product = commerce_product_load($product_id);
  if (empty($product)) {
    watchdog('considine', 'Bad card product (!product_id) during populate process.', array(), array('!product_id' => $product_id));
  }
  if (!empty($product) && $product->type == CONSIDINE_CARD_PRODUCT_TYPE) {
    $card_product = $card_wrapper->field_card_product->value();
    if (empty($card_product)) {
      $card_wrapper->field_card_product = $product;
    }
  }
  // Card step.
  $step = $card_wrapper->field_card_step->value();
  // If there is no steps - add first from the list.
  if (empty($step)) {
    $card_product = $card_wrapper->field_card_product->value();
    if (!empty($card_product)) {
      $default_step = $card_wrapper->field_card_product->field_steps[0]->value();
      if (!empty($default_step)) {
        $card_wrapper->field_card_step = $default_step;
      }
    }
  }
  // Card status populate
  $status = $card_wrapper->field_card_status->value();
  if (empty($status)) {
    $card_wrapper->field_card_status = CONSIDINE_CARD_STATUS_NEW;
  }
  // Card quantity.
  $default_quantity = variable_get('considine_card_default_quantity', 100);
  $quantity = $card_wrapper->field_card_quantity->value();
  if (empty($quantity)) {
    $quantity = $card_wrapper->field_card_quantity = $default_quantity;
  }
}

/**
 * Check if user already passed step during workflow.
 * @param $card
 * @param $step
 */
function considine_card_step_passed($card, $step) {
  $current_step = considine_card_current_step($card);
  $steps = considine_card_steps($card);
  $steps_keys = array_keys($steps);
  $current_step_position = array_search($current_step->nid, $steps_keys);
  $step_position = array_search($step->nid, $steps_keys);
  if ($current_step_position <= $step_position || $current_step_position == 0) {
    return FALSE;
  }
  else {
    return TRUE;
  }
}

/**
 * Function to get card current step.
 * @param $card
 * @param $active
 */
function considine_card_current_step($card, $active = TRUE) {
  $step = NULL;
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  // Getting card product.
  $step = $card_wrapper->field_card_step->value();
  if ($active && !$step->status) {
    return NULL;
  }
  return $step;
}

function considine_card_workflow_page($card) {
  $page = array(
    'current' => array(),
    'next' => array(),
    'prev' => array()
  );
  $current_step = considine_card_current_step($card);
  $steps = array_values(considine_card_steps($card));
  foreach ($steps as $key => $step) {
    if (empty($step)) {
      continue;
    }
    if ($step->nid == $current_step->nid) {
      $page['current'] = array(
        'step_id' => $step->nid,
        'name' => $step->title,
        'type' => $current_step->type
      );
      if (isset($steps[$key + 1]) && !empty($steps[$key + 1])) {
        $next_step = $steps[$key + 1];
        $page['next'] = array(
          'step_id' => $next_step->nid,
          'name' => $next_step->title,
          'type' => $next_step->type
        );
      }
      if (isset($steps[$key - 1]) && !empty($steps[$key - 1])) {
        $prev_step = $steps[$key - 1];
        $page['prev'] = array(
          'step_id' => $prev_step->nid,
          'name' => $prev_step->title,
          'type' => $prev_step->type
        );
      }
      break;
    }
  }
  return $page;
}

/**
 * Function to get list of card steps.
 * @param $card
 * @param $active
 */
function considine_card_steps($card, $active = TRUE) {
  $steps = array();
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  // Getting card product.
  $card_product = $card_wrapper->field_card_product->value();
  if (!empty($card_product)) {
    // List through card product steps.
    $card_product_wrapper = $card_wrapper->field_card_product;
    foreach ($card_product_wrapper->field_steps as $step_wrapper) {
      $step = $step_wrapper->value();
      if (!empty($step)) {
        $status = $step->status;
        if ($active && !$status) {
          continue;
        }
        $steps[$step->nid] = $step;
      }
    }
  }
  return $steps;
}

/**
 * Function to get step from steps list.
 * @param $card
 * @param $step_name
 */
function considine_card_steps_step($card, $step_type) {
  $steps = considine_card_steps($card);
  if (empty($steps)) {
    return NULL;
  }
  $search_step = NULL;
  foreach ($steps as $step) {
    if ($step->type == $step_type) {
      $search_step = $step;
      break;
    }
  }
  return $search_step;
}

/**
 * Function to get list of step options.
 * @param $card
 */
function considine_card_step_options($step) {
  $step_wrapper = entity_metadata_wrapper('node', $step);
  $step_option_field = 'field_' . $step->type . '_options';
  $options = array();
  if (!isset($step_wrapper->{$step_option_field})) {
    return $options;
  }
  $options = $step_wrapper->{$step_option_field}->value();
  return $options;
}

/**
 * Function to check if option is active.
 */
function considine_card_step_option_active($card, $option, $step_type) {
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  // Getting workflow info.
  $info = considine_card_workflow_item($step_type);
  if (empty($info)) {
    return FALSE;
  }

  // Getting option field name.
  $card_option_field = $info['card_option_field'];
  // Getting card option value.
  if (isset($card_wrapper->{$card_option_field})) {
    $card_option = $card_wrapper->{$card_option_field}->value();
    if (!empty($card_option) && $card_option->item_id == $option->item_id) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 * Function to check if option item is active.
 */
function considine_card_step_option_item_active($card, $option, $step_type) {
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  // Getting workflow info.
  $info = considine_card_workflow_item($step_type);
  if (empty($info)) {
    return FALSE;
  }
  // Getting option item field name.
  $card_option_item_field = $info['card_option_field_item'];
  // Getting card option item value.
  if (isset($card_wrapper->{$card_option_item_field})) {
    $card_option_item = $card_wrapper->{$card_option_item_field}->value();
    if (!empty($card_option_item) && $card_option_item->item_id == $option->item_id) {
      return TRUE;
    }
  }
  return FALSE;
}

function considine_card_step_flip_options($card, $step) {
  $colors = array();
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $card_back_id = $card_wrapper->field_card_back->raw();
  $step_wrapper = entity_metadata_wrapper('node', $step);
  $items = $step_wrapper->field_flip_options->value();
  if (!empty($items)) {
    foreach ($items as $item) {
      $item_id = $item->item_id;
      $item_wrapper = entity_metadata_wrapper('field_collection_item', $item);
      $color_web = $item_wrapper->field_color->value();
      $colors[$item_id]['color'] = $color_web;
      // Color name.
      $color_name = $item_wrapper->field_color_name->value();
      $colors[$item_id]['name'] = $color_name;
      // Color item ID.
      $colors[$item_id]['back_id'] = $item_id;
      // Color paper variation image.
      $colors[$item_id]['img'] = '';
      $image = $item_wrapper->field_image->value();
      if (!empty($image)) {
        $uri = $image['uri'];
        $colors[$item_id]['img'] = considine_card_step_option_image($uri);
      }
      if (!empty($card_back_id) && $item_id == $card_back_id) {
        $colors[$item_id]['active'] = 'active';
      }
      else {
        $colors[$item_id]['active'] = '';
      }
    }
    return $colors;
  }

  return FALSE;
}

function considine_card_step_flip_default($step) {
  $default = '';
  $step_wrapper = entity_metadata_wrapper('node', $step);
  if (isset($step_wrapper->field_flip_default)) {
    $default_flip = $step_wrapper->field_flip_default->value();
    if (!empty($default_flip)) {
      $default = $default_flip->item_id;
    }
  }
  return $default;
}

function considine_card_step_flip($card) {
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $card_type_option = $card_wrapper->field_card_type->value();
  if (!empty($card_type_option)) {
    $card_type = $card_wrapper->field_card_type->field_option_type->value();
    if ($card_type == 'flip') {
      return TRUE;
    }
  }
  return FALSE;
}

function considine_card_step_option_image($uri) {
  return image_style_url(CONSIDINE_CARD_WORKFLOW_IMAGE_STYLE, $uri);
}

function considine_card_step_option_default_image($option, $style = '') {
  $option_wrapper = entity_metadata_wrapper('field_collection_item', $option);
  $image = $option_wrapper->field_option_image->value();
  if (!empty($image)) {
    $uri = $image['uri'];
    if (!empty($style)) {
      return image_style_url($style, $uri);
    }
    return file_create_url($uri);
  }
  return '';
}

function considine_card_quantities() {
  $quantity_string = variable_get('considine_card_quantities', '50, 100, 150');
  $list = explode(',', $quantity_string);
  $quantities = array();
  foreach ($list as $item) {
    $quantities[] = trim($item);
  }
  return $quantities;
}

function considine_card_envelope_quantities() {
  $quantity_string = variable_get('considine_card_envelope_quantities', '50, 100, 150, 250');
  $list = explode(',', $quantity_string);
  $quantities = array(0);
  foreach ($list as $item) {
    $quantity = trim($item);
    $quantities[$quantity] = $quantity;
  }
  return $quantities;
}

/**
 * Helper function to get card progress items.
 */
function considine_card_progress_item($step_wrapper, $current_step) {
  // Progress configuration.
  $link_attributes = array();
  if ($current_step->nid == $step_wrapper->nid->value()) {
    $link_attributes['class'] = array('active');
  }
  $step_name = $step_wrapper->title->value();
  $progress_item = array(
    'content' => '<li ' . drupal_attributes($link_attributes) . '><span>' . $step_name . '</span>'
  );
  return $progress_item;
}

function considine_card_next_button($variables) {
  $next = $variables['page']['next'];
  $prev = $variables['page']['prev'];
  $saved = TRUE;
  if (isset($variables['saved'])) {
    $saved = $variables['saved'];
  }
  $content = '';
  if (!empty($prev)) {
    $prev_text  = $prev['name'];
    $content .= '<a click-redirect="#card-workflow-submit-prev" class="select-prev-btn" href="#"><span>' . $prev_text . '</span></a>';
  }
  if (!empty($next)) {
    $next_text  = $next['name'];
    if (!empty($next['type']) && $next['type'] == 'cstep_order') {
      $next_text = t("Review");
    }
    $content .= '<a click-redirect="#card-workflow-submit-next" class="select-next-btn" href="#"><span>' . $next_text . '</span></a>';
  }
  else {
    if ($saved) {
      $content .= '<a click-redirect="#card-workflow-submit-next" class="select-next-btn workflow-finish-btn" href="#"><span>' . t('Order') . '</span></a>';
    }
    else {
      $content .= '<a class="select-next-btn workflow-finish-btn" href="#" popup-opener="#card-save-popup"><span>' . t('Order') . '</span></a>';
    }
  }

  return $content;
}

function considine_card_slider_controls($variables) {
  $content = '<a class="prev-btn" href="#">' . t('prev') . '</a>';
  $content .= '<a class="next-btn" href="#">' . t('next') . '</a>';
  return $content;
}

function considine_card_generate_url($card, $type) {
  $path = 'print-card/' . $card->entityform_id;
  $query = array();
  $query['type'] = $type;
  $url = url($path, array('absolute' => TRUE, 'query' => $query));
  return $url;
}

function considine_card_generate_file_name($card, $type = 'image', $order_id = NULL, $item_id = 0) {
  $time = REQUEST_TIME;
  if ($type == 'pdf') {
    if ($order_id && $item_id) {
      $name = sprintf('%1$08d_%2$03d', $order_id, $item_id);
    }
    else {
      $name = 'card-' . $card->entityform_id . '.pdf';
    }
  }
  if ($type == 'image') {
    $name = 'card-' . $card->entityform_id . '-' . $time . '.jpg';
  }
  return $name;
}

function considine_card_generate($card, $order_id, $item_id) {
  $image_generate_enable = variable_get('considine_generate_image_enable', TRUE);
  $pdf_generate_enable = variable_get('considine_generate_pdf_enable', TRUE);
  // Generate card image.
  if ($image_generate_enable) {
    considine_card_generate_image($card);
  }
  // Generate card pdf.
  if ($pdf_generate_enable) {
    considine_card_queue_card_pdf($card, $order_id, $item_id);
  }
}

function considine_card_generate_image($card, $order_id = NULL) {
  $library = libraries_load('phpwkhtmltopdf');
  libraries_load_files($library);
  // Check temp directory to generate in.
  $tmp_dir = "temporary://card/images";
  file_prepare_directory($tmp_dir, FILE_CREATE_DIRECTORY);
  // Getting card print URL.
  // Must be used in printing process.
  $url = considine_card_generate_url($card, 'image');
  // Generate file name.
  $file_name = considine_card_generate_file_name($card, 'image');
  // Getting card tmp file path.
  $file_path_tmp = $tmp_dir . '/' . $file_name;
  // Generate image into tmp directory.
  $options = array(
    'encoding' => 'UTF-8',
    'width' => 525,
    'disable-smart-width',
    'quality' => 5
  );
  $result = FALSE;
  try {
    $image =new WkHtmlToImage();
    $image->setOptions($options);
    $image->setPage($url);
    $result = $image->saveAs($file_path_tmp);
  }
  catch (Exception $e) {
    watchdog('considine', 'WkHtmlToImage error - !exception.', array(), array('!exception' => $e->getMessage()));
  }

  // Save image into card.
  if ($result) {
    // Generate file object.
    $image_file_path = drupal_realpath($file_path_tmp);
    $file = (object) array(
      'uid' => $card->uid,
      'uri' => $image_file_path,
      'filemime' => file_get_mimetype($image_file_path),
      'status' => 1,
    );
    // Check normal file directory.
    $file_dir = 'public://card/images';
    file_prepare_directory($file_dir, FILE_CREATE_DIRECTORY);
    // Copy file from tmp directory to normal.
    $file = file_copy($file, $file_dir, FILE_EXISTS_REPLACE);
    // Flush image styles
    considine_card_flush_images($file);
    // Save copied file into card object.
    $card->field_card_image[LANGUAGE_NONE][0] = (array)$file;
    entityform_save($card);
    // TODO: add card image deleting from TMP directory.
  }
}

function considine_card_generate_pdf($card, $order_id = NULL, $item_id = 0) {
  $library = libraries_load('phpwkhtmltopdf');
  libraries_load_files($library);
  // Check temp directory to generate in.
  $tmp_dir = "temporary://card/pdfs";
  file_prepare_directory($tmp_dir, FILE_CREATE_DIRECTORY);
  // Getting card print URL.
  // Must be used in printing process.
  $url = considine_card_generate_url($card, 'pdf');
  // Generate file name.
  $file_name = considine_card_generate_file_name($card, 'pdf', $order_id, $item_id);
  // Getting card tmp file path.
  $file_path_tmp = $tmp_dir . '/' . $file_name;
  // Generate image into tmp directory.
  $options = array(
    'encoding' => 'UTF-8',
    'page-height' => '4.5inch',
    'page-width' => '7inch',
    'dpi' => 300,
    'margin-top' => 0,
    'margin-left' => 0,
    'margin-right' => 0,
    'margin-bottom' => 0,
    'print-media-type',
  );
  $smart = variable_get('considine_card_wkhtmltopdf_smart', TRUE);
  if ($smart) {
    $options[] = 'enable-smart-shrinking';
  }
  else {
    $options[] = 'disable-smart-shrinking';
  }
  try {
    //$pdf = new Pdf($path);
    $pdf = new WkHtmlToPdf();
    $pdf->setOptions($options);
    $pdf->addPage($url);
    $result = $pdf->saveAs($file_path_tmp);
    $error = $pdf->getError();
    if (!empty($error)) {
      watchdog('considine', 'WkHtmlToPdf error - !error.', array(), array('!error' => $error));
    }
  }
  catch (Exception $e) {
    watchdog('considine', 'WkHtmlToPdf error - !exception.', array(), array('!exception' => $e->getMessage()));
  }

  // Save image into card.
  if ($result) {
    // Generate file object.
    $pdf_file_path = drupal_realpath($file_path_tmp);
    $file = (object) array(
      'uid' => $card->uid,
      'uri' => $pdf_file_path,
      'display' => 1,
    );
    // Check normal file directory.
    if (!empty($order_id)) {
      $file_dir = 'public://card/pdfs/' . $order_id;
    }
    else {
      $file_dir = 'public://card/pdfs';
    }

    file_prepare_directory($file_dir, FILE_CREATE_DIRECTORY);
    // Copy file from tmp directory to normal.
    $file = file_copy($file, $file_dir, FILE_EXISTS_REPLACE);
    // Save copied file into card object.
    $card->field_card_pdf[LANGUAGE_NONE][0] = (array)$file;

    entityform_save($card);
    // TODO: add card image deleting from TMP directory.
    return $result;
  }
  return FALSE;
}

function considine_card_flush_image_styles() {
  $styles = array(
    'basket_card',
    'user_card',
    'thumbnail'
  );
  return $styles;
}

/**
 * Regenarate image styles images.
 */
function considine_card_flush_images($file, $styles = array()) {
  if (empty($styles)) {
    $styles = considine_card_flush_image_styles();
  }
  $uri = $file->uri;
  foreach ($styles as $style_name) {
    $style = image_style_load($style_name);
    $derivative_uri = image_style_path($style['name'], $uri);
    file_unmanaged_delete($derivative_uri);
    image_style_create_derivative($style, $uri, $derivative_uri);
    image_style_flush($style);
  }
}

/**
 * Gets card type code.
 */
function considine_card_get_type($card) {
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $type_item = $card_wrapper->field_card_type->value();
  $type = '';
  if (!empty($type_item)) {
    $type_item_wrapper = $card_wrapper->field_card_type;
    $type = $type_item_wrapper->field_option_type->value();
  }
  return $type;
}

/**
 * Helper function to get font css styles as json.
 * Copy of original fontyourface_font_css function.
 */
function considine_card_fontyourface_font_json($font) {

  $css = array();

  if ($font->css_family) {

    if ($font->css_family[0] === "'") {
      $family_list = $font->css_family;
    }
    else {
      $family_list = "'" . $font->css_family . "'";
    }

    if ($font->css_fallbacks) {
      $family_list .= ', ' . $font->css_fallbacks;
    }

    $css['font-family'] = $family_list;

  }

  if ($font->css_style) {
    $css['font-style'] = $font->css_style;
  }

  if ($font->css_weight) {
    $css['font-weight'] = $font->css_weight;
  }

  if ($font->css_size) {
    $css['font-size'] = $font->css_size . 'px';
  }

  if ($font->css_line_height) {
    $css['line-height'] = $font->css_line_height . '%';
  }

  return drupal_json_encode($css);

}

/**
 * Fill XML template with data.
 */
function considine_card_fill_xml($data) {
  $xml = file_get_contents(drupal_get_path('module', 'considine_card') . '/templates/export.xml');
  $data = (array) $data;
  return format_string($xml, $data);
}

/**
 * Export card and envelope data based on line item.
 */
function considine_card_export_line_item($line_item, $save = TRUE) {
  if (!in_array($line_item->type, array('card', 'envelope'))) {
    return;
  }
  $data = array(
    '!THUMBNAIL!' => '',
    '!ORDER_NUMBER!' => '',
    '!ITEM_NUMBER!' => '',
    '!ORDERED_DATE!' => '',
    '!QUANTITY!' => '',
    '!ITEM_PRICE!' => '',
    '!ORDER_PRICE!' => '',
    '!PRODUCT_NAME!' => '',
    '!PRODUCT_CODE!' => '',
    '!PRODUCT_DESCRIPTION!' => '',
    '!USER_EMAIL!' => '',
    '!USER_NAME!' => '',
    '!FONT_COLOR!' => '',
    '!BORDER_COLOR!' => '',
    '!MOTIF!' => '',
    '!BACKGROUND_COLOR!' => '',
    '!BACK_COLOR!' => '',
    '!TELEPHONE1!' => '',
    '!SHIPPING_ADDRESS!' => ''
  );

  $line_item_wrapper = entity_metadata_wrapper('commerce_line_item', $line_item);
  $card = $line_item_wrapper->field_card->value();
  if (empty($card)) {
    watchdog('considine', 'Export failed for line_item - !line_item. Card empty.', array('!line_item' => $line_item->line_item_id), WATCHDOG_ERROR);
    return;
  }
  $order = commerce_order_load($line_item->order_id);
  if (empty($order)) {
    watchdog('considine', 'Export failed for line_item - !line_item. Order empty.', array('!line_item' => $line_item->line_item_id), WATCHDOG_ERROR);
    return;
  }
  $order_wrapper = entity_metadata_wrapper('commerce_order', $order);
  $data['!ORDER_NUMBER!'] = $order->order_number;
  $data['!QUANTITY!'] = $line_item->quantity;
  $data['!USER_EMAIL!'] = $order->mail;
  $order_total = $order_wrapper->commerce_order_total->value();
  $data['!ORDER_PRICE!'] = considine_card_export_format_price($order_total);
  $line_item_total = $line_item_wrapper->commerce_total->value();
  $data['!ITEM_PRICE!'] = considine_card_export_format_price($line_item_total);
  $data['!ORDERED_DATE!'] = date('d/m/Y H:i:s', $order->created);
  // Delivery address
  $delivery_address_wrapper = $order_wrapper->commerce_customer_shipping;
  if (!empty($delivery_address_wrapper)) {
    $address = $delivery_address_wrapper->commerce_customer_address->value();
    $data['!USER_NAME!'] = $address['name_line'];
    $address_dispaly = array();
    $address_dispaly[] = check_plain($address['name_line']);
    $address_dispaly[] = check_plain($address['thoroughfare']) . ' ' . check_plain($address['premise']);
    $address_dispaly[] = check_plain($address['administrative_area']);
    $address_dispaly[] = check_plain($address['locality']);
    $address_dispaly[] = check_plain($address['postal_code']);
    $country_list = country_get_list();
    $address_dispaly[] = $country_list[$address['country']] . ' (' . $address['country'] . ')';
    $data['!SHIPPING_ADDRESS!'] = implode(" \n", $address_dispaly);
    $data['!TELEPHONE1!'] = $delivery_address_wrapper->field_phone_number->value(array('sanitize' => TRUE));
  }

  $card_wrapper = $line_item_wrapper->field_card;
  $card = $card_wrapper->value();
  if ($line_item->type == 'card') {
    $data['!PRODUCT_CODE!'] = considine_card_export_product_code($card);
    // Getting card paper info.
    $paper = $card_wrapper->field_card_paper->value();
    if (!empty($paper)) {
      $paper_wrapper = $card_wrapper->field_card_paper;
      $data['!BACKGROUND_COLOR!'] = trim($paper_wrapper->field_panton_color->value(array('sanitize' => TRUE)));
    }
    // Getting card border info.
    $border = $card_wrapper->field_card_border->value();
    if (!empty($border)) {
      $border_wrapper = $card_wrapper->field_card_border;
      $data['!BORDER_COLOR!'] = trim($border_wrapper->field_panton_color->value(array('sanitize' => TRUE)));
    }
    // Getting card font info.
    $font_color = $card_wrapper->field_card_font_color->value();
    if (!empty($font_color)) {
      $font_color_wrapper = $card_wrapper->field_card_font_color;
      $data['!FONT_COLOR!'] = trim($font_color_wrapper->field_panton_color->value(array('sanitize' => TRUE)));
    }
    // Getting card back info (if card flip)
    if (considine_card_step_flip($card)) {
      $back = $card_wrapper->field_card_back->value();
      if (!empty($back)) {
        $back_wrapper = $card_wrapper->field_card_back;
        $data['!BACK_COLOR!'] = trim($back_wrapper->field_panton_color->value(array('sanitize' => TRUE)));
      }
    }
    $motif_option = $card_wrapper->field_card_motif_option->value();
    if (!empty($motif_option)) {
      $motif_option_wrapper = $card_wrapper->field_card_motif_option;
      $motif_wrapper = $card_wrapper->field_card_motif;
      $motif_name = trim($motif_option_wrapper->field_option_name->value(array('sanitize' => TRUE)));
      $motif_color = $motif_wrapper->field_panton_color->value();
      $data['!MOTIF!'] = $motif_name . '_' . $motif_color;
    }
    // Getting card name
    $data['!PRODUCT_NAME!'] = $card_wrapper->field_card_name->value(array('sanitize' => TRUE));
    // Getting card type info.
    $data['!PRODUCT_DESCRIPTION!'] = trim($card_wrapper->field_card_type->field_option_name->value(array('sanitize' => TRUE)));
    $card_image = $card_wrapper->field_card_image->value();
    if (!empty($card_image)) {
      $data['!THUMBNAIL!'] = file_create_url($card_image['uri']);
    }
    // Setting filename to export.
    $file_name = 'card-' . $card_wrapper->entityform_id->value();

  }
  if ($line_item->type == 'envelope') {
    $data['!PRODUCT_CODE!'] = considine_card_export_product_code($card, 'envelope');
    $data['!PRODUCT_NAME!'] = $card_wrapper->field_card_envelope_option->field_option_name->value(array('sanitize' => TRUE));
    $data['!PRODUCT_DESCRIPTION!'] = $card_wrapper->field_card_envelope_option->field_option_description->value(array('sanitize' => TRUE));
    $file_name = 'envelope-' . $card_wrapper->entityform_id->value();
  }

  $xml  = considine_card_fill_xml($data);
  if (!$save) {
    return array(
      'content' => $xml,
      'name' => $file_name
    );
  }
  $file_dir = 'public://card/xml';
  file_prepare_directory($file_dir, FILE_CREATE_DIRECTORY);
  file_unmanaged_save_data($xml , $file_dir . '/' . $file_name . '.xml', FILE_EXISTS_REPLACE);
}

/**
 * Helper function to format price for export.
 */
function considine_card_export_format_price($price) {
  $currency = commerce_currency_load($price['currency_code']);
  $amount = commerce_currency_amount_to_decimal($price['amount'], $price['currency_code']);
  $out = number_format(commerce_currency_round(abs($amount), $currency), $currency['decimals'], $currency['decimal_separator'], $currency['thousands_separator']);
  return $out;
}

/**
 * Helper function to get export card product codes.
 */
function considine_card_export_product_code($card, $type = 'card') {
  $out = '';
  $code = array();
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  if ($type == 'card') {
    $code['type'] = $card_wrapper->field_card_type->field_option_name->value(array('sanitize' => TRUE));
    if (considine_card_step_flip($card)) {
      $code['paper'] = 'Flip';
      $code['color'] = $card_wrapper->field_card_back->field_color_name->value(array('sanitize' => TRUE));
    }
    else {
      $code['paper'] = $card_wrapper->field_card_paper_option->field_option_name->value(array('sanitize' => TRUE));
      $code['color'] = $card_wrapper->field_card_paper->field_color_name->value(array('sanitize' => TRUE));
    }

    $out = implode('-', array_map('trim', $code));
  }
  if ($type == 'envelope') {
    $out = trim($card_wrapper->field_card_envelope_option->field_option_name->value(array('sanitize' => TRUE)));
  }
  return $out;
}
