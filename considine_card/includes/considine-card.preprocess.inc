<?php

/**
 * @file
 * Defines additional preprocess functions for Considine card workflow.
 */


/**
 * Implements hook_preprocess_entity().
 */
function considine_card_preprocess_entity(&$variables) {
  if ($variables['entity_type'] == 'entityform') {
    $entityform = $variables['entityform'];
    if ($entityform->type == 'card') {
      $view_mode = $variables['view_mode'];
      $attr_function = 'considine_card_view_' . $view_mode . '_attributes';
      if (function_exists($attr_function)) {
        $variables['view_attributes'] = $attr_function($entityform);
      }
      // Setting template class.
      $variables['classes_array'] = array('card-wrapper');
      $card_class = considine_card_get_type($entityform);
      $variables['classes_array'][] = 'card-' . $card_class . '-template';

      if ($view_mode == 'card_workflow') {
        $entityform_wrapper = entity_metadata_wrapper('entityform', $entityform);
        $step = $entityform_wrapper->field_card_step->value();
        if (!empty($step)) {
          $variables['theme_hook_suggestions'][] = 'entityform__' . $entityform->type . '__' . $step->type;
        }
        $current_step = $entityform_wrapper->field_card_step->value();
        if ($current_step->type == 'cstep_paper') {
          $flip = considine_card_step_flip($entityform);
          $flip_enable = variable_get('considine_card_flip_enable', FALSE);
          if ($flip && $flip_enable) {
            $variables['theme_hook_suggestions'][] = 'entityform__' . $entityform->type . '__flip';
          }
        }
        if (in_array($card_class, array('flip', 'liner')) && $step->type != 'cstep_type') {
          unset($variables['content']['field_card_body']);
        }
      }
      if ($view_mode == 'full') {
        $query_params = drupal_get_query_parameters();
        if (isset($query_params['type']) && $query_params['type'] == 'image') {
           $variables['classes_array'][] = 'card-print-image';
        }
      }
    }
  }
}

/**
 * Function to get special attributes for card view display.
 * Only style and sprcific angular attributes must be used. No classes.
 */
function considine_card_view_full_attributes($card) {
  $attributes = array(
    'type' => array(),
    'paper' => array(),
    'border' => array(),
    'logo' => array(),
    'text1' => array(),
    'text2' => array(),
    'back' => array()
  );
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  // Getting paper.
  $paper = $card_wrapper->field_card_paper->value();
  if (!empty($paper)) {
    $paper_file = $card_wrapper->field_card_paper->field_image->file->value();
    if (!empty($paper_file)) {
      $paper_uri = $paper_file->uri;
      $attributes['paper'] = array(
        'style' => 'background: url(' . file_create_url($paper_uri) . ');',
      );
    }
  }
  // Getting border.
  $border = $card_wrapper->field_card_border->value();
  if (!empty($border)) {
    $border_file = $card_wrapper->field_card_border->field_image->file->value();
    if (!empty($border_file)) {
      $border_uri = $border_file->uri;
      $attributes['border'] = array(
        'style' => 'background: url(' . file_create_url($border_uri) . ');',
      );
    }
  }
  // Getting text attributes.
  $font = $card_wrapper->field_card_font->value();
  if (!empty($font)) {
    // Getting text color.
    $text_color = '';
    $font_color = $card_wrapper->field_card_font_color->value();
    if (!empty($font_color)) {
      $text_color = $card_wrapper->field_card_font_color->field_color->value();
    }
    // Getting text 1 attributes.
    $font_id1 = $font->field_option_font1[LANGUAGE_NONE][0]['fid'];
    $font_size = $font->field_option_font_size[LANGUAGE_NONE][0]['value'];
    $font_height = $font->field_option_font_height[LANGUAGE_NONE][0]['value'];
    $size_styles = '';
    if ($font_size) {
      $size_styles .= ' font-size:' . $font_size . 'px;';
    }
    if ($font_height) {
      $size_styles .= ' line-height:' . $font_height . '%;';
    }
    $font1 = fontyourface_get_font($font_id1);
    if (!empty($font1)) {
      $font1_css = fontyourface_font_css($font1);
      if (!empty($font1_css)) {
        $attributes['text1'] = array(
          'style' => $font1_css . ' color:' . $text_color . ';' . $size_styles,
        );
      }
    }
    // Getting text 2 attributes.
    $font_id2 = $font->field_option_font2[LANGUAGE_NONE][0]['fid'];
    $font2 = fontyourface_get_font($font_id2);
    if (!empty($font2)) {
      $font2_css = fontyourface_font_css($font2);
      if (!empty($font2_css)) {
        $attributes['text2'] = array(
          'style' => $font2_css . ' color:' . $text_color . ';' . $size_styles,
        );
      }
    }
  }
  // Getting motif.
  $motif = $card_wrapper->field_card_motif->value();
  if (!empty($motif)) {
    $motif_file = $card_wrapper->field_card_motif->field_image->file->value();
    if (!empty($motif_file)) {
      $motif_uri = $motif_file->uri;
      $attributes['logo'] = file_create_url($motif_uri);
    }
  }
  // Getting back.
  $flip = considine_card_step_flip($card);
  $flip_enable = variable_get('considine_card_flip_enable', FALSE);
  $query_params = drupal_get_query_parameters();
  if ($flip && $flip_enable && ($query_params['type'] != 'image')) {
    $back = $card_wrapper->field_card_back->value();
    if (!empty($back)) {
      $back_file = $card_wrapper->field_card_back->field_image->file->value();
      if (!empty($back_file)) {
        $back_uri = $back_file->uri;
        $attributes['back'] = array(
          'style' => 'background: url(' . file_create_url($back_uri) . ');',
        );
      }
    }
  }
  return $attributes;
}

/**
 * Function to get special attributes for card view display.
 * Only style and sprcific angular attributes must be used. No classes.
 */
function considine_card_view_card_workflow_attributes($card) {
  $attributes = array(
    'type' => array(),
    'paper' => array(),
    'border' => array(),
    'logo' => array(),
    'text1' => array(),
    'text2' => array()
  );
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  // Getting paper.
  $paper = $card_wrapper->field_card_paper->value();
  if (!empty($paper)) {
    $paper_file = $card_wrapper->field_card_paper->field_image->file->value();
    if (!empty($paper_file)) {
      $paper_uri = $paper_file->uri;
      $attributes['paper'] = array(
        'style' => 'background: url(' . image_style_url(CONSIDINE_CARD_WORKFLOW_IMAGE_STYLE, $paper_uri) . ');',
      );
    }
  }
  // Getting border.
  $border = $card_wrapper->field_card_border->value();
  if (!empty($border)) {
    $border_file = $card_wrapper->field_card_border->field_image->file->value();
    if (!empty($border_file)) {
      $border_uri = $border_file->uri;
      $attributes['border'] = array(
        'style' => 'background: url(' . image_style_url(CONSIDINE_CARD_WORKFLOW_IMAGE_STYLE, $border_uri) . ');',
      );
    }
  }
  // Getting text attributes.
  $font = $card_wrapper->field_card_font->value();
  if (empty($font)) {
    $card = $card_wrapper->value();
    $type_step = considine_card_steps_step($card, 'cstep_type');
    if (!empty($type_step)) {
      if (!empty($type_step->field_step_default_font[LANGUAGE_NONE][0]['target_id'])) {
        $default_font_id = $type_step->field_step_default_font[LANGUAGE_NONE][0]['target_id'];
        $font = field_collection_item_load($default_font_id);
      }
    }
  }
  if (!empty($font)) {
    // Getting text color.
    $text_color = '';
    $font_color = $card_wrapper->field_card_font_color->value();
    if (!empty($font_color)) {
      $text_color = $card_wrapper->field_card_font_color->field_color->value();
    }

    // Getting text 1 attributes.
    $font_id1 = $font->field_option_font1[LANGUAGE_NONE][0]['fid'];
    $font_size = $font->field_option_font_size[LANGUAGE_NONE][0]['value'];
    $font_height = $font->field_option_font_height[LANGUAGE_NONE][0]['value'];
    $size_styles = '';
    if ($font_size) {
      $size_styles .= ' font-size:' . $font_size . 'px;';
    }
    if ($font_height) {
      $size_styles .= ' line-height:' . $font_height . '%;';
    }
    $font1 = fontyourface_get_font($font_id1);
    if (!empty($font1)) {
      $font1_css = fontyourface_font_css($font1);
      if (!empty($font1_css)) {
        $attributes['text1'] = array(
          'style' => $font1_css . ' color:' . $text_color . ';' .  $size_styles,
        );
      }
    }
    // Getting text 2 attributes.
    $font_id2 = $font->field_option_font2[LANGUAGE_NONE][0]['fid'];
    $font2 = fontyourface_get_font($font_id2);
    if (!empty($font2)) {
      $font2_css = fontyourface_font_css($font2);
      if (!empty($font2_css)) {
        $attributes['text2'] = array(
          'style' => $font2_css . ' color:' . $text_color . ';' . $size_styles,
        );
      }
    }
  }

  // Getting motif.
  $motif = $card_wrapper->field_card_motif->value();
  if (!empty($motif)) {
    $motif_file = $card_wrapper->field_card_motif->field_image->file->value();
    if (!empty($motif_file)) {
      $motif_uri = $motif_file->uri;
      $attributes['logo'] = image_style_url(CONSIDINE_CARD_WORKFLOW_IMAGE_STYLE, $motif_uri);
    }
  }
  return $attributes;
}

/**
 * Implements hook_preprocess_node().
 */
function considine_card_preprocess_node(&$variables) {
  $type = $variables['type'];
  $part = "cstep_";
  if (strpos($type, $part) === 0) {

    $view_mode = $variables['view_mode'];
    $node = $variables['node'];
    if ($view_mode == 'card_workflow' && arg(0) == 'create-card') {
      // Setting step price.
      $price_array = considine_card_cstep_get_price($node);
      if (!empty($price_array)) {
        $price = commerce_currency_format($price_array['amount'], $price_array['currency_code']);
        $variables['content']['price'] = array(
          '#markup' => $price
        );
      }

      // Setting slider controls.
      $variables['content']['slider_controls'] = array(
        '#markup' => theme('considine_card_slider_controls')
      );
      $card_id = key($variables['content']['card']['entityform']);
      $card = entityform_load($card_id);
      $card_wrapper = entity_metadata_wrapper('entityform', $card);
      $page = considine_card_workflow_page($card);
      // Setting next button.
      $variables['content']['next_button'] = array(
        '#markup' => theme('considine_card_next_button', array('page' => $page))
      );
    }

  }
}
