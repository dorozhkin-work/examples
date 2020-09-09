<?php

/**
 * @file
 * Plugin to handle the Considine card wizzard form.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Considine card form'),
  'description' => t('Add\'s Considinecard wizzard form.'),
  'category' => t('Considine'),
  'render callback' => 'considine_card_form_pane_render',
  'required context' => new ctools_context_required(t('Entityform'), 'entityform')
);

/**
 * Output function for the considine card form pane.
 */
function considine_card_form_pane_render($subtype, $conf, $panel_args, $contexts) {
  global $user;
  module_load_include('inc', 'entityform', 'entityform.admin');
  $block = new stdClass();

  // Getting card.
  $card = NULL;
  if (!empty($contexts->data)) {
    $card = $contexts->data;
  }

  if (empty($card)) {
    // Check correct param.
    $arg = arg(1);
    if (!empty($arg) && !is_numeric($arg)) {
      $block->content = '';
      return $block;
    }

    $card = considine_card_card_new($user->uid);

    entityform_save($card);

    drupal_goto('create-card/' . $card->entityform_id);
  }

  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  // Getting product information.
  $card_product = $card_wrapper->field_card_product->value();
  // Getting current step.
  $current_step = $card_wrapper->field_card_step->value();

  considine_card_recalculate_price($card);

  entityform_save($card);

  $progress_items = array();
  $options = array();
  $card_product_wrapper = entity_metadata_wrapper('commerce_product', $card_product);
  // Workflow name.
  $workflow_name = $card_product_wrapper->title->value();

  foreach ($card_product_wrapper->field_steps as $step_wrapper) {
    $status = $step_wrapper->status->value();
    if (!$status) {
      continue;
    }

    $progress_items[] = considine_card_progress_item($step_wrapper, $current_step);
  }


  $option_fn = "considine_card_" . $current_step->type . "_options";
  if (function_exists($option_fn)) {
    // Options configuration.
    $options = $option_fn($current_step, $card_wrapper->value());
  }

  if (!empty($options)) {
    // Options display.
    drupal_add_js('var ' . $options['name'] . '=' . drupal_json_encode($options['options']) . ';',array('type' => 'inline', 'scope' => 'header', 'group' => JS_LIBRARY, 'weight' => -10));
  }
  // Step display.
  $node_view = node_view($current_step, 'card_workflow');
  // Card display
  $card_content = entityform_page_view($card, 'card_workflow');

  //$card_view = drupal_render($card_content);
  $node_view['card'] = $card_content;
  $card_price_array = $card_wrapper->field_card_price->value();
  $node_view['card']['card_price'] = array(
    '#markup' => commerce_currency_format($card_price_array['amount'], $card_price_array['currency_code'])
  );
  // Form display.
  $form = drupal_get_form($card->type . '_entityform_edit_form', $card, 'submit', 'page');
  //$form = drupal_get_form('considine_card_workflow_form', $card);
  $node_view['card_form'] = $form;

  $content = drupal_render($node_view);

  $content = array(
    'header' => theme('considine_card_workflow_header', array('name' => $workflow_name)),
    'progress' => theme('considine_card_workflow_progress', array('items' => $progress_items)),
    'content' => $content,
    'form' => drupal_render($form)
  );
  $block->content = theme('considine_card_workflow', $content);
  return $block;
}
