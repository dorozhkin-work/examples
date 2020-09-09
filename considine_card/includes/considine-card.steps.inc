<?php

/**
 * @file
 * Defines additional functions for Considine card steps functionality.
 */

/**
 * Function to get base step options.
 */
function considine_card_step_option_base_values($option) {
  $option_wrapper = entity_metadata_wrapper('field_collection_item', $option);
  $data = array();
  if (isset($option_wrapper->field_option_name)) {
    $data['title'] = $option_wrapper->field_option_name->value();
  }
  if (isset($option_wrapper->field_option_description)) {
    $data['description'] = $option_wrapper->field_option_description->value();
  }
  return $data;
}

function considine_card_step_option_price_values($prices, $quantity = 0) {
  if (empty($prices)) {
    return (object)$prices;
  }
  $price_options = array();
  foreach ($prices as $price_quantity => $price) {
    $price_options[$price_quantity] = array(
      'quantity' => $price_quantity,
      'price' => $price,
      'active' => ($quantity == $price_quantity) ? 'active' : '',
    );
  }
  return (object)$price_options;
}

/**
 * Helper function to get card type step options.
 */
function considine_card_cstep_type_options($step, $card) {
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $quantity = (int)$card_wrapper->field_card_quantity->value();
  // Default options.
  $defualt_option = array(
    'title' => '',
    'description' => '',
    'img' => '',
    'active' => '',
    'prices' => '',
    'option_id' => '_none',
    'type_class' => ''
  );
  $options = array('name' => 'cardTypes');
  $step_options = considine_card_step_options($step);
  $active_option = 0;

  // Getting correct prices for first step.
  $card_prices = array();
  $quantities = considine_card_quantities();
  foreach ($quantities as $q) {
    $changed_card = clone($card);
    $changed_card_wrapper = entity_metadata_wrapper('entityform', $changed_card);
    $changed_card_wrapper->field_card_quantity = $q;
    considine_card_recalculate_price($changed_card);
    $changed_prices = considine_card_step_price_list($changed_card, $step);
    foreach ($changed_prices as $key => $changed_price) {
      $card_prices[$key][$q] = $changed_price[$q];
    }
  }

  if (!empty($step_options)) {
    foreach ($step_options as $step_option) {
      $option_id = $step_option->item_id;
      $option_wrapper = entity_metadata_wrapper('field_collection_item', $step_option);
      // Setting default values.
      $options['options'][$option_id] = $defualt_option;
      $options['options'][$option_id]['option_id'] = $option_id;
      // Setting base values.
      $base_values = considine_card_step_option_base_values($step_option);
      $options['options'][$option_id]  = array_merge($options['options'][$option_id], $base_values);
      // Setting image
      $image = $option_wrapper->field_option_image->value();
      if (!empty($image)) {
        $options['options'][$option_id]['img'] = image_style_url(CONSIDINE_CARD_WORKFLOW_IMAGE_STYLE, $image['uri']);
      }
      // Setting prices.
      $prices = considine_card_step_option_price_values($card_prices[$option_id], $quantity);
      $options['options'][$option_id]['prices'] = $prices;
      // Setting type class
      $type_classes = array();
      $type_classes[] = 'card-wrapper';
      $card_type =  $option_wrapper->field_option_type->value();
      if (!empty($card_type)) {
        $type_classes[] = 'card-' . $card_type . '-template';
        $options['options'][$option_id]['type_class'] = $type_classes;
      }
      // Setting active status.
      $active = FALSE;
      $active = considine_card_step_option_active($card, $step_option, $step->type);
      if ($active) {
        $active_option = $option_id;
        $options['options'][$option_id]['active'] = 'active';
      }
    }
  }

  if (empty($active_option)) {
    reset($options['options']);
    $first_key = key($options['options']);
    $options['options'][$first_key]['active'] = 'active';
  }

  return $options;
}

function considine_card_cstep_words_options($step, $card) {
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $options = array('name' => 'yourText');
  $options['options']['text1'] = $card_wrapper->field_card_title->value();
  $options['options']['text2'] = $card_wrapper->field_card_body->value();
  return $options;
}


/**
 * Helper function to get card paper step options.
 */
function considine_card_cstep_paper_options($step, $card) {
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  // Card current quantity.
  $quantity = (int)$card_wrapper->field_card_quantity->value();
  // Getting card saved color id.
  $card_color_id = $card_wrapper->field_card_paper->raw();

  $flip = considine_card_step_flip($card);
  $flip_enable = variable_get('considine_card_flip_enable', FALSE);
  if ($flip && $flip_enable) {
    $options = array('name' => 'backTypes');
    $options['options']['colors'] = considine_card_step_flip_options($card, $step);
    $options['options']['default'] = considine_card_step_flip_default($step);
    return $options;
  }
  // Default options.
  $defualt_option = array(
    'title' => '',
    'description' => '',
    'img' => '',
    'active' => '',
    'price' => '',
    'colors' => '',
    'option_id' => '_none'
  );
  $options = array('name' => 'paperTypes');
  $step_options = considine_card_step_options($step);
  $active_option = 0;
  if (!empty($step_options)) {
    foreach ($step_options as $step_option) {
      $option_id = $step_option->item_id;
      $option_wrapper = entity_metadata_wrapper('field_collection_item', $step_option);
      // Setting default values.
      $options['options'][$option_id] = $defualt_option;
      $options['options'][$option_id]['option_id'] = $option_id;
      // Setting base values.
      $base_values = considine_card_step_option_base_values($step_option);
      $options['options'][$option_id]  = array_merge($options['options'][$option_id], $base_values);
      // Setting prices.
      $price_list = considine_card_step_price_list($card, $step, $option_id);
      $options['options'][$option_id]['price'] = $price_list[$quantity];
      // Paper option calor image variations.
      $colors = considine_card_cstep_paper_options_colors($step_option, $card_color_id);
      $options['options'][$option_id]['colors'] = $colors;
      // Active option.
      if (!empty($colors)) {
        foreach ($colors as $color) {
          if ($color['active'] == 'active') {
            $active_option = $option_id;
            $options['options'][$option_id]['active'] = 'active';
          }
        }
      }
      // Default image
      $options['options'][$option_id]['default_img'] = considine_card_step_option_default_image($step_option);
    }
  }

  // If there is no default option.
  if (empty($active_option)) {
    // Setting first option.
    reset($options['options']);
    $option_first_key = key($options['options']);
    $options['options'][$option_first_key]['active'] = 'active';
    // Setting first paper item.
    reset($options['options'][$option_first_key]['colors']);
    $color_first_key = key($options['options'][$option_first_key]['colors']);
    $options['options'][$option_first_key]['colors'][$color_first_key]['active'] = 'active';
    $default_color = $options['options'][$option_first_key]['colors'][$color_first_key];
    $options['options'][$option_first_key]['active'] = 'active';
  }

  return $options;
}

function considine_card_cstep_paper_options_colors($option, $card_color_id = 0) {
  $option_wrapper = entity_metadata_wrapper('field_collection_item', $option);
  $papers = $option_wrapper->field_option_paper->value();
  $colors = array();
  if (empty($papers)) {
    return $colors;
  }

  foreach ($papers as $key => $paper) {
    if (empty($paper)) {
      continue;
    }
    $color = array();
    $item_id = $paper->item_id;
    $delta = $paper->delta();

    $color['delta'] = $key;
    $paper_wrapper = entity_metadata_wrapper('field_collection_item', $paper);
    // Color value.
    $color_web = $paper_wrapper->field_color->value();
    $color['color'] = $color_web;
    // Color name.
    $color_name = $paper_wrapper->field_color_name->value();
    $color['name'] = $color_name;
    // Color item ID.
    $color['paper_id'] = $item_id;
    // Color paper variation image.
    $color['img'] = '';
    $image = $paper_wrapper->field_image->value();
    if (!empty($image)) {
      $uri = $image['uri'];
      $color['img'] = considine_card_step_option_image($uri);
    }
    // Checking active item.
    $color['active'] = '';
    if ($item_id == $card_color_id) {
      $color['active'] = 'active';
    }
    $colors[$delta] = $color;
  }
  return $colors;
}

/**
 * Helper function to get card font face step options.
 */
function considine_card_cstep_font_options($step, $card) {
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $quantity = (int)$card_wrapper->field_card_quantity->value();
  $options = array('name' => 'fontTypes');
  $card_font_id = $card_wrapper->field_card_font->raw();
  if (empty($card_font_id)) {
    $type_step = considine_card_steps_step($card, 'cstep_type');
    if (!empty($type_step)) {
      if (!empty($type_step->field_step_default_font[LANGUAGE_NONE][0]['target_id'])) {
        $default_font_id = $type_step->field_step_default_font[LANGUAGE_NONE][0]['target_id'];
        $card_font_id = $default_font_id;
      }
    }
  }
  $step_options = considine_card_step_options($step);
  foreach ($step_options as $option) {
    $option_id = $option->item_id;
    $options['options']['fonts'][$option_id]['title'] = considine_card_cstep_font_options_font($option, 'title');
    $options['options']['fonts'][$option_id]['content'] = considine_card_cstep_font_options_font($option, 'body');
    $price_list = considine_card_step_price_list($card, $step, $option_id);
    $options['options']['fonts'][$option_id]['price'] = $price_list[$quantity];
    $options['options']['fonts'][$option_id]['active'] = '';
    if (!empty($card_font_id) && $card_font_id == $option_id) {
      $options['options']['fonts'][$option_id]['active'] = 'active';
    }
  }
  // Getting colors.
  $card_color_id = $card_wrapper->field_card_font_color->raw();
  $options['options']['colors'] = considine_card_cstep_font_options_color($step, $card_color_id);

  return $options;
}

/**
 * Gets font information for font face step option.
 */
function considine_card_cstep_font_options_font($option, $type = 'title') {
  if ($type == 'title') {
    $default_font_css = array(
      'font-family' => 'Georgia, sans-serif',
      'font-style'  => 'italic'
    );
    $font_id = $option->field_option_font1[LANGUAGE_NONE][0]['fid'];
  }
  if ($type == 'body') {
    $default_font_css = array(
      'font-family' =>  'Arial, sans-serif',
      'font-style' =>  'normal'
      );
    $font_id = $option->field_option_font2[LANGUAGE_NONE][0]['fid'];
  }
  $style = '';
  $font = fontyourface_get_font($font_id);
  $option_wrapper = entity_metadata_wrapper('field_collection_item', $option);
  // Font size.
  $font_size = $option_wrapper->field_option_font_size->value();
  $font->css_size = (!empty($font_size)) ? $font_size : '';
  // Font line height.
  $font_height = $option_wrapper->field_option_font_height->value();
  $font->css_line_height = (!empty($font_height)) ? $font_height : '';
  // Getting json.
  $font_css = considine_card_fontyourface_font_json($font);
  //$font_css = fontyourface_font_css($font);
  if (!empty($font_css)) {
    $style = $font_css;
  }
  else {
    $style = drupal_json_encode($default_font_css);
  }
  return $style;
}

/**
 * Gets color information for font face step option.
 */
function considine_card_cstep_font_options_color($step, $card_color_id = 0) {
  $step_wrapper = entity_metadata_wrapper('node', $step);
  $colors = array();
  foreach ($step_wrapper->field_cstep_font_colors as $color_wrapper) {
    $color = $color_wrapper->value();
    if (empty($color)) {
      continue;
    }
    $color_id = $color_wrapper->item_id->value();
    $colors[$color_id] = array(
      'color' => $color_wrapper->field_color->value(),
      'name' => $color_wrapper->field_color_name->value(),
      'active' => '',
      'color_id' => $color_id
    );
    if (!empty($card_color_id) && $color_id == $card_color_id) {
      $colors[$color_id]['active'] = 'active';
    }
  }
  return $colors;
}


/**
 * Helper function to get card border step options.
 */
function considine_card_cstep_border_options($step, $card) {
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $quantity = (int)$card_wrapper->field_card_quantity->value();
  $card_price = considine_card_get_price($card);
  $step_price = considine_card_step_price($card, $step);
  // Default options.
  $defualt_option = array(
    'title' => '',
    'description' => '',
    'active' => '',
    'colors' => '',
    'option_id' => ''
  );
  $card_option_id = $card_wrapper->field_card_border_option->raw();
  $card_color_id = $card_wrapper->field_card_border->raw();
  $options = array('name' => 'borderTypes');
  $step_options = considine_card_step_options($step);
  $active_option = 0;
  foreach ($step_options as $step_option) {
    $option_id = $step_option->item_id;
    $option_wrapper = entity_metadata_wrapper('field_collection_item', $step_option);
    // Setting default values.
    $options['options']['items'][$option_id] = $defualt_option;
    $options['options']['items'][$option_id]['option_id'] = $option_id;
    // Setting base values.
    $base_values = considine_card_step_option_base_values($step_option);
    $options['options']['items'][$option_id]  = array_merge($options['options']['items'][$option_id], $base_values);

    $price_list = considine_card_step_price_list($card, $step, $option_id);
    $options['options']['items'][$option_id]['price'] = $price_list[$quantity];

    $colors = considine_card_cstep_border_options_colors($step_option, $card_color_id);
    $options['options']['items'][$option_id]['colors'] = $colors;

    if (!empty($card_option_id) && $card_option_id == $option_id) {
      $active_option = $option_id;
      $options['options']['items'][$option_id]['active'] = 'active';
      foreach ($colors as $color) {
        if ($color['active'] == 'active') {
          $options['options']['items'][$option_id]['activeColor'] = $color['color'];
          $options['options']['items'][$option_id]['activeName'] = $color['name'];
          $options['options']['items'][$option_id]['borderImage'] = $color['img'];
        }
      }
    }
    else {
      reset($options['options']['items'][$option_id]['colors']);
      $color_first_key = key($options['options']['items'][$option_id]['colors']);
      $options['options']['items'][$option_id]['activeColor'] = $colors[$color_first_key]['color'];
      $options['options']['items'][$option_id]['activeName'] = $colors[$color_first_key]['name'];
      $options['options']['items'][$option_id]['borderImage'] = $colors[$color_first_key]['img'];
    }
  }

  $options['options']['defaultPrice'] = commerce_currency_format($card_price['amount'] - $step_price['amount'], $card_price['currency_code']);


  return $options;
}

function considine_card_cstep_border_options_colors($option, $card_color_id = 0) {
  $option_wrapper = entity_metadata_wrapper('field_collection_item', $option);
  $colors = array();
  foreach ($option_wrapper->field_option_border as $border_wrapper) {
    $border = $border_wrapper->value();
    if (empty($border)) {
      continue;
    }
    $item_id = $border_wrapper->item_id->value();
    $colors[$item_id]['name'] = $border_wrapper->field_color_name->value();
    $colors[$item_id]['color'] = $border_wrapper->field_color->value();
    $colors[$item_id]['img'] = '';

    $image = $border_wrapper->field_image->value();
    if (!empty($image)) {
      $uri = $image['uri'];
      $colors[$item_id]['img'] = considine_card_step_option_image($uri);
    }
    $colors[$item_id]['active'] = '';
    if (!empty($card_color_id) && $item_id == $card_color_id) {
      $colors[$item_id]['active'] = 'active';
    }
  }
  return $colors;
}


/**
 * Helper function to get card motif step options.
 */
function considine_card_cstep_motif_options($step, $card) {

  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $quantity = (int)$card_wrapper->field_card_quantity->value();
  $card_price = considine_card_get_price($card);
  $step_price = considine_card_step_price($card, $step);
  $card_motif_id = $card_wrapper->field_card_motif->raw();
  $card_option_id = $card_wrapper->field_card_motif_option->raw();
  $font_color = '';
  $font_color_value = $card_wrapper->field_card_font_color->value();
  if (!empty($font_color_value)) {
    $font_color = $card_wrapper->field_card_font_color->field_color->value();
  }

  $options = array('name' => 'motifTypes');
  // Default options.
  $defualt_option = array(
    'title' => '',
    'description' => '',
    'active' => '',
    'option_id' => '_none',
    'price' => '',
    'motif' => '',
    'motif_id' => '_none'
  );
  // Empty option.
  $options['options']['empty'] = array(
    'title' => '',
    'description' => '',
    'active' => '',
    'option_id' => '_none',
    'price' => commerce_currency_format($card_price['amount'] - $step_price['amount'], $card_price['currency_code']),
    'motif' => '',
    'motif_id' => '_none'
  );
  $step_options = considine_card_step_options($step);
  $step_prices = considine_card_step_price_list($card, $step);
  foreach ($step_options as $step_option) {
    $option_id = $step_option->item_id;
    $option_wrapper = entity_metadata_wrapper('field_collection_item', $step_option);
    // Setting default values.
    $options['options']['items'][$option_id] = $defualt_option;
    $options['options']['items'][$option_id]['option_id'] = $option_id;
    // Setting base values.
    $base_values = considine_card_step_option_base_values($step_option);
    $options['options']['items'][$option_id]  = array_merge($options['options']['items'][$option_id], $base_values);
    // Setting prices.
    $options['options']['items'][$option_id]['price'] = $step_prices[$option_id][$quantity];
    // Setting motif options.
    $motif_options = considine_card_cstep_motif_options_motif($step_option, $font_color, $card_motif_id);
    $options['options']['items'][$option_id]  = array_merge($options['options']['items'][$option_id], $motif_options);
    // Default image
    $options['options']['items'][$option_id]['default_img'] = considine_card_step_option_default_image($step_option, 'card_workflow_motif_list');
    // If there are no motif with related color or default - delete item.
    if ($options['options']['items'][$option_id]['motif'] == '_none' || $options['options']['items'][$option_id]['motif_id'] == '_none') {
      unset($options['options']['items'][$option_id]);
    }
  }

  return $options;
}

function considine_card_cstep_motif_options_motif($option, $color, $card_motif_id = 0) {
  $item = array();

  $option_wrapper = entity_metadata_wrapper('field_collection_item', $option);
  foreach ($option_wrapper->field_option_motif as $motif_wrapper) {
    $item_id = $motif_wrapper->item_id->value();
    $motif_color = $motif_wrapper->field_color->value();

    $item['motif'] = '';
    $item['motif_id'] = '_none';
    $item['active'] = '';
    // Check if motif can be added into options list.
    if (!empty($motif_color) && !empty($color) && drupal_strtolower($motif_color) == drupal_strtolower($color)) {
      $image = $motif_wrapper->field_image->value();
      if (!empty($image)) {
        $uri = $image['uri'];
        $item['motif'] = considine_card_step_option_image($uri);
      }
      $item['motif_id'] = $item_id;
      if (!empty($card_motif_id) && $card_motif_id == $item_id) {
        $item['active'] = 'active';
      }
      break;
    }
  }
  return $item;
}

/**
 * Helper function to get card envelope step options.
 */
function considine_card_cstep_envelope_options($step, $card) {

  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $quantity = (int)$card_wrapper->field_card_envelope_quantity->value();

  // Getting paper color.
  $paper_color = '';
  $paper = $card_wrapper->field_card_paper->value();
  if (!empty($paper)) {
    $paper_color = $card_wrapper->field_card_paper->field_color->value();
  }

  $options = array('name' => 'envelopeTypes');
  $step_options = considine_card_step_options($step);
  $active_option = 0;
  if (!empty($step_options)) {
    foreach ($step_options as $step_option) {
      $option_id = $step_option->item_id;
      $option_wrapper = entity_metadata_wrapper('field_collection_item', $step_option);
      // Setting default values.
      $options['options'][$option_id] = considine_card_cstep_envelope_options_default();
      // Setting option id
      $options['options'][$option_id]['option_id'] = $option_id;
      // Setting base values.
      $base_values = considine_card_step_option_base_values($step_option);
      $options['options'][$option_id]  = array_merge($options['options'][$option_id], $base_values);
      // Setting prices.
      $price_list = considine_card_step_envelope_price_list($card, $step, $option_id);
      $prices = considine_card_step_option_price_values($price_list, $quantity);
      $options['options'][$option_id]['prices'] = $prices;
      // Setting active status.
      $card_envelope_option = $card_wrapper->field_card_envelope_option->raw();
      if ($card_envelope_option == $option_id) {
        $active_option = $option_id;
        $options['options'][$option_id]['active'] = 'active';
      }
      // Getting envelopes data.
      $envelopes = $option_wrapper->field_option_envelope->value();
      if (!empty($envelopes)) {
        $item = considine_card_cstep_envelope_options_item($envelopes, $paper_color);
        if (!empty($item)) {
          $options['options'][$option_id]['img'] = $item['img'];
          $options['options'][$option_id]['envelope_id'] = $item['envelope_id'];
        }
        else {
          unset($options['options'][$option_id]);
        }
      }
    }
  }
  // If no default value - take first.
  if (empty($active_option)) {
    reset($options['options']);
    $first_option = key($options['options']);
    $options['options'][$first_option]['active'] = 'active';
  }
  // Setting empty option.
  //$options['options']['_none'] = considine_card_cstep_envelope_options_empty();

  return $options;
}

function considine_card_cstep_envelope_options_default() {
  $defualt_option = array(
    'title' => '',
    'description' => '',
    'option_id' => '_none',
    'active' => '',
    'envelope_id' => '_none',
    'img' => '',
    'prices' => ''
  );
  return $defualt_option;
}

function considine_card_cstep_envelope_options_empty() {
  $option = considine_card_cstep_envelope_options_default();
  $quantities = considine_card_envelope_quantities();
  $empty_prices = array();
  $amount = 0;
  if (!empty($quantities)) {
    foreach ($quantities as $quantity) {
      $empty_prices[$quantity] = array(
        'price' => commerce_currency_format($amount, commerce_default_currency()),
        'quantity' => $quantity,
        'active' => ''
      );
    }
  }
  $option['title'] = t('No envelope');
  $option['description'] = t('You can select this option to not use envelopes');
  $option['prices'] = (object)$empty_prices;
  return $option;
}

/**
 * Helper function to get card envelope step options item.
 */
function considine_card_cstep_envelope_options_item($envelopes = array(), $color = '') {
  $item = array();
  if (empty($envelopes)) {
    return $item;
  }
  foreach ($envelopes as $envelope) {
    $envelope_wrapper = entity_metadata_wrapper('field_collection_item', $envelope);
    $item_id = $envelope_wrapper->item_id->value();
    $envelope_color = $envelope_wrapper->field_color->value();
    if (!empty($color) && drupal_strtolower($envelope_color) != drupal_strtolower($color)) {
      continue;
    }
    $image = $envelope_wrapper->field_image->value();
    if (!empty($image)) {
      $uri = $image['uri'];
      $item['envelope_id'] = $item_id;
      $item['img'] = considine_card_step_option_image($uri);
    }
  }
  // If no color match - return first envelope.
  // if (empty($item)) {
  //   return considine_card_cstep_envelope_options_item(array($envelopes[0]));
  // }
  return $item;
}


/**
 * Helper function to get card order step options.
 */
function considine_card_cstep_order_options($step, $card) {
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $flip = considine_card_step_flip($card);
  $options = array('name' => 'cardData');
  $options['options']['backImage'] = '';
  if ($flip) {
    $back_image_item = $card_wrapper->field_card_back->value();
    if (!empty($back_image_item)) {
      $back_image_wrapper = $card_wrapper->field_card_back;
      $image = $back_image_wrapper->field_image->value();
      if (!empty($image)) {
        $uri = $image['uri'];
        $render = considine_card_step_option_image($uri);
        $options['options']['backImage'] = $render;
      }
    }
  }
  $card_price = considine_card_get_price($card);
  $envelope_price = considine_card_get_envelope_price($card);
  $options['options']['price'] = commerce_currency_format($envelope_price['amount'] + $card_price['amount'], commerce_default_currency());
  return $options;
}
