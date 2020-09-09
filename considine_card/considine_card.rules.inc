<?php

/**
 * @file
 * Rules integration for considine card.
 *
 */


/**
 * Implements hook_rules_action_info().
 */
function considine_card_rules_action_info() {
  $actions = array();

  $actions['considine_card_export_action'] = array(
    'label' => t("Export card or envelope info into XML"),
    'parameter' => array(
      'commerce_line_item' => array(
        'type' => 'commerce_line_item',
        'label' => t('Line item'),
      ),
    ),
    'group' => t('Considine'),
  );

  return $actions;
}

/**
 * Rules action: export card information based on line item.
 */
function considine_card_export_action($line_item) {
  if (!in_array($line_item->type, array('envelope', 'card'))) {
    return;
  }
  considine_card_export_line_item($line_item);
}
