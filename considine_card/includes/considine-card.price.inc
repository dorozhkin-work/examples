
<?php

/**
 * @file
 * Defines additional price functions for Considine card workflow.
 */

function  considine_card_price($card) {
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $steps = considine_card_steps($card);
  $amount = 0;
  if (!empty($step)) {
    return $amount;
  }
  foreach ($steps as $step) {
    $step_price = considine_card_step_price($card, $step);
  }
}

/**
 * Return list of options prices based on card quantity variants.
 * Every price amount already calculated.
 * @param  $card
 * @param  $step
 * @param  $option_id
 */
function considine_card_step_price_list($card, $step, $option_id = 0) {
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  // Card current price amount.
  $current_price = considine_card_get_price($card);
  $current_amount = $current_price['amount'];
  // Step price by default or already saved.
  $step_price = considine_card_step_price($card, $step);
  $step_amount = $step_price['amount'];
  // Base amount of card price without current option price.
  $base_amount = $current_amount - $step_amount;
  $prices = array();
  // Getting quantity array.
  $quantity_array = considine_card_quantities();
  // Getting step options.
  $options = considine_card_step_options($step);
  foreach ($options as $option) {
    $prices[$option->item_id] = array();
    foreach ($quantity_array as $quantity) {
      $option_amount = considine_card_step_option_price_amount($option, $quantity);
      $amount = $option_amount + $base_amount;
      $prices[$option->item_id][$quantity] = commerce_currency_format($amount, $step_price['currency_code']);
    }
  }

  if (!empty($option_id)) {
    if (!empty($prices[$option_id])) {
      return $prices[$option_id];
    }
    else {
      return array();
    }
  }

  return $prices;
}

function considine_card_step_envelope_price_list($card, $step, $option_id) {

  $card_price = considine_card_get_price($card);

  $prices = array();
  // Getting quantity array.
  $quantity_array = considine_card_envelope_quantities();
  // Getting step options.
  $options = considine_card_step_options($step);
  foreach ($options as $option) {
    $prices[$option->item_id] = array();
    foreach ($quantity_array as $quantity) {
      $amount = considine_card_step_option_price_amount($option, $quantity);
      $prices[$option->item_id][$quantity] = commerce_currency_format($amount + $card_price['amount'], commerce_default_currency());
    }
  }
  if (!empty($option_id)) {
    if (!empty($prices[$option_id])) {
      return $prices[$option_id];
    }
    else {
      return array();
    }
  }
  return $prices;
}

/**
 * Return card step price based on current saved card step option.
 * If card has saved option for current step - returns price of saved option
 * based on card quantity.
 * If card has no saved option for current step - returns minimal price from all
 * step options based on card quantiity.
 * Returns commerce price array.
 * @param  $card
 * @param  $step
 */
function considine_card_step_price($card, $step) {
  $amount = 0;
  // configure default price array.
  $currency_code = commerce_default_currency();
  $price = array(
    'amount' => $amount,
    'currency_code' => $currency_code,
    'data' => array(),
  );

  $base_price = array(
    'amount' => $amount,
    'currency_code' => $currency_code,
    'data' => array(),
  );

  $price['data'] = commerce_price_component_add($price, 'base_price', $base_price, TRUE);

  $step_type = $step->type;
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $quantity = (int)$card_wrapper->field_card_quantity->value();

  // Getting workflow info.
  $info = considine_card_workflow_item($step_type);
  if (empty($info)) {
    return $price;
  }
  // Getting option field name.
  $card_option_field = $info['card_option_field'];
  // Getting card option value.
  $card_option = NULL;
  if (isset($card_wrapper->{$card_option_field})) {
    $card_option = $card_wrapper->{$card_option_field}->value();
  }

  // If no option save in card  - use step minimal price option.
  if (!empty($card_option)) {
    $amount = considine_card_step_option_price_amount($card_option, $quantity);
  }
  else {
    $step_passed = considine_card_step_passed($card, $step);
    if ($step_passed) {
      $amount = 0;
    }
    else {
      if (!in_array($step->type, array('cstep_border', 'cstep_motif'))) {
        $amount = considine_card_step_prices_minimal($step, $quantity);
      }
      else {
        $amount = 0;
      }
    }
  }
  // Setting correct price datas.
  $price['amount'] = $amount;
  $base_price['amount'] = $amount;
  $price['data'] = array();
  $price['data'] = commerce_price_component_add($price, 'base_price', $base_price, TRUE);

  return $price;
}

/**
 * Returns card envelope step saved option price.
 * Returns commerce price array.
 * @param  $card
 */
function considine_card_step_envelope_price($card) {
  $amount = 0;
  // configure default price array.
  $currency_code = commerce_default_currency();
  $price = array(
    'amount' => $amount,
    'currency_code' => $currency_code,
    'data' => array(),
  );

  $base_price = array(
    'amount' => $amount,
    'currency_code' => $currency_code,
    'data' => array(),
  );

  $price['data'] = commerce_price_component_add($price, 'base_price', $base_price, TRUE);

  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $quantity = (int)$card_wrapper->field_card_envelope_quantity->value();

  $card_option_field = 'field_card_envelope_option';
  // Getting card option value.
  if (isset($card_wrapper->{$card_option_field})) {
    $card_option = $card_wrapper->{$card_option_field}->value();
    if (!empty($card_option)) {
      $amount = considine_card_step_option_price_amount($card_option, $quantity);
    }
  }
  // Setting correct price datas.
  $price['amount'] = $amount;
  $base_price['amount'] = $amount;
  $price['data'] = array();
  $price['data'] = commerce_price_component_add($price, 'base_price', $base_price, TRUE);

  return $price;
}

/**
 * Function to get step minimal price based on quantity.
 * @param $step
 * @param $quantity
 */
function considine_card_step_prices_minimal($step, $quantity = 0) {
  if (empty($quantity)) {
    return 0;
  }
  $prices = considine_card_step_prices($step, $quantity);
  $prices = array_filter($prices);
  if (empty($prices)) {
    return 0;
  }
  return min($prices);
}

/**
 * Function to get step prices based on quantity.
 * @param $step
 * @param $quantity
 */
function considine_card_step_prices($step, $quantity = 0) {
  $prices = array();
  if (empty($quantity)) {
    return $prices;
  }
  $options = considine_card_step_options($step);
  if (empty($options)) {
    return $prices;
  }

  foreach ($options as $option) {
    $prices[$option->item_id] = considine_card_step_option_price_amount($option, $quantity);
  }
  return $prices;
}

/**
 * Function to get step option price based on quantity.
 * @param $option
 * @param $quantity
 */
function considine_card_step_option_price_amount($option, $quantity = 0) {
  $option_wrapper = entity_metadata_wrapper('field_collection_item', $option);
  $amount = 0;
  if (!isset($option_wrapper->field_option_prices)) {
    return $amount;
  }
  if (empty($quantity)) {
    return $amount;
  }
  $minimal_amount = 0;
  foreach ($option_wrapper->field_option_prices as $price_wrapper) {
    $price_amount = $price_wrapper->field_price->amount->value();
    if (empty($minimal_amount) || $minimal_amount > $price_amount) {
      $minimal_amount = $price_amount;
    }
    $price_quantity = (int)$price_wrapper->field_quantity->value();
    if ($quantity >= $price_quantity) {
      $amount = $price_wrapper->field_price->amount->value();
    }
  }
  if (empty($amount)) {
    $amount = $minimal_amount;
  }
  return $amount;
}

/**
 * Function to get card price component.
 * @param $card
 */
function considine_card_get_price($card, $quantity = 0) {
  $wrapper = entity_metadata_wrapper('entityform', $card);
  // By default give card traight price value.
  if (empty($quantity)) {
    $price = $wrapper->field_card_price->value();
    return $price;
  }
  $card_quantity = $wrapper->field_card_quantity->value();
  if ($card_quantity == $quantity) {
    $price = $wrapper->field_card_price->value();
  }
  else {
    $changed_card = clone($card);
    $changed_card_wrapper = entity_metadata_wrapper('entityform', $changed_card);
    $changed_card_wrapper->field_card_quantity = $quantity;
    considine_card_recalculate_price($changed_card);
    $price = $changed_card_wrapper->field_card_price->value();
  }

  return $price;
}

/**
 * Function to get card price component.
 * @param $card
 */
function considine_card_get_envelope_price($card, $quantity = 0) {
  $wrapper = entity_metadata_wrapper('entityform', $card);
  if (empty($quantity)) {
    $price = $wrapper->field_card_envelope_price->value();
    return $price;
  }
  $envelope_quantity = $wrapper->field_card_envelope_quantity->value();
  if ($envelope_quantity == $quantity) {
    $price = $wrapper->field_card_envelope_price->value();
  }
  else {
    $changed_card = clone($card);
    $changed_card_wrapper = entity_metadata_wrapper('entityform', $changed_card);
    $changed_card_wrapper->field_card_envelope_quantity = $quantity;
    considine_card_recalculate_envelope_price($changed_card);
    $price = $changed_card_wrapper->field_card_envelope_price->value();
  }
  return $price;

}


/**
 * Function to recalculate card price.
 * @param $card
 */
function considine_card_recalculate_price($card) {
  // Getting card information.
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  // Before recalculation - price must be 0.
  $currency_code = commerce_default_currency();
  $base_price = array(
    'amount' => 0,
    'currency_code' => $currency_code,
    'data' => array(),
  );
  $card_wrapper->field_card_price->data = commerce_price_component_add($base_price, 'base_price', $base_price, TRUE);
  $card_wrapper->field_card_price->amount = 0;
  $card_wrapper->field_card_price->currency_code = $currency_code;

  // Getting steps prices amount.
  $steps = considine_card_steps($card);
  if (empty($steps)) {
    watchdog('considine', t('Card !card_id has no related steps during reculculation'), array('!card_id' => $card->entityform_id));
    return;
  }
  $amount = 0;
  foreach ($steps as $step) {
    // Skip envelope step on price calculation.
    if ($step->type == 'cstep_envelope') {
      considine_card_recalculate_envelope_price($card);
      continue;
    }

    $step_price = considine_card_step_price($card, $step);
    // Convert the steps total to the order's currency for totalling
    $component_total = commerce_price_component_total($step_price);
    $amount += commerce_currency_convert(
      $component_total['amount'],
      $component_total['currency_code'],
      $currency_code
    );
    // Combine the steps total's component prices into the card total.
    $card_wrapper->field_card_price->data = commerce_price_components_combine(
      $card_wrapper->field_card_price->value(),
      $step_price
    );
  }
  $card_wrapper->field_card_price->amount = $amount;
}

/**
 * Function to recalculate card envelope price.
 * @param $card
 */
function considine_card_recalculate_envelope_price($card) {
  // Getting card information.
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  // Before recalculation - price must be 0.
  $currency_code = commerce_default_currency();
  // Set zero envelope price before calculation.
  $card_wrapper->field_card_envelope_price->amount = 0;
  $card_wrapper->field_card_envelope_price->currency_code = $currency_code;
  $base_price = array(
    'amount' => 0,
    'currency_code' => $currency_code,
    'data' => array(),
  );
  $card_wrapper->field_card_envelope_price->data = commerce_price_component_add($base_price, 'base_price', $base_price, TRUE);
  $envelope_price = considine_card_step_envelope_price($card);
  // Convert the envelope step total to the card currency for totalling
  $component_total = commerce_price_component_total($envelope_price);
  $envelope_amount = commerce_currency_convert(
    $component_total['amount'],
    $component_total['currency_code'],
    $currency_code
  );

  // Combine the envelope step total's component prices into the card envelope total.
  $card_wrapper->field_card_envelope_price->data = commerce_price_components_combine(
    $card_wrapper->field_card_envelope_price->value(),
    $envelope_price
  );
  $card_wrapper->field_card_envelope_price->amount = $envelope_amount;
}

/**
 * Helper function to get card step price.
 */
function considine_card_cstep_get_price($step) {
  $step_wrapper = entity_metadata_wrapper('node', $step);
  // Prices options.
  $quantity = variable_get('considine_card_default_quantity', 100);
  $price_array = array();
  if (isset($step_wrapper->field_cstep_prices)) {
    foreach ($step_wrapper->field_cstep_prices as $price_wrapper) {
      $price_quantity = $price_wrapper->field_quantity->value();
      if ($quantity >= $price_quantity) {
        $price_array = $price_wrapper->field_price->value();
      }
    }
    // Get first price if no default assigned.
    if (empty($price_array)) {
      $price_array = $step_wrapper->field_cstep_prices[0]->field_price->value();
    }
  }
  // Need to add base_price component.
  // Used in card recalculation function.
  if (!empty($price_array)) {
    $currency_code = $price_array['currency_code'];
    $base_price = array(
      'amount' => $price_array['amount'],
      'currency_code' => $currency_code,
      'data' => array(),
    );
    $price_array['data'] = commerce_price_component_add($price_array, 'base_price', $base_price, TRUE);
  }

  return $price_array;
}
