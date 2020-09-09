<?php

/**
 * @file
 * Administration pages for Expay module.
 */

/**
 * Administration settings form.
 */
function commerce_expay_admin_settings_form($form, &$form_state) {
  // Expay request mode.
  $mod_options = array(
    'prod' => t('Prod'),
    'sandbox' => t('Sandbox')
  );
  $form['commerce_expay_host'] = array(
    '#title' => t('Work mode'),
    '#type' => 'select',
    '#options' => $mod_options,
    '#description' => t('Chose work mode.'),
    '#default_value' => variable_get('commerce_expay_host', 'sandbox'),
  );
  $form['commerce_expay_host']['#description'] .= '<br>' . t('When you activate the "sandbox", all transactions will be the test, under the regime of "prod", all transactions will be real.');
  // Expay merchant ID.
  $form['commerce_expay_merchant_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Merchant key'),
    '#size' => 40,
    '#default_value' => variable_get('commerce_expay_merchant_id', ''),
    '#description' => t('Merchant key in Server system.'),
  );
  // Expay secret key.
  $form['commerce_expay_merchant_sk'] = array(
    '#type' => 'textfield',
    '#title' => t('Secret key'),
    '#size' => 40,
    '#default_value' => variable_get('commerce_expay_merchant_sk', ''),
    '#description' => t('Secret key in Server system.'),
  );
  // List of allowed IPs.
  $form['commerce_expay_allowed_ips'] = array(
    '#type' => 'textfield',
    '#title' => t('IP Server'),
    '#size' => 40,
    '#default_value' => variable_get('commerce_expay_allowed_ips', '127.0.0.1'),
    '#description' => t('IP address of request Server system. You could use several IP addresses with "," sign. For example : 192.168.11.1, 127.0.0.1.'),
  );
  // Debug mode.
  $form['commerce_expay_debug_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Debug mode'),
    '#default_value' => variable_get('commerce_expay_debug_mode', 0),
  );
  // Use only local logos.
  $form['commerce_expay_logos_local_only'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use fetched logos'),
    '#default_value' => variable_get('commerce_expay_logos_local_only', 0),
  );
  // Expay system background callback URL.
  $form['commerce_expay_callback_url'] = array(
    '#type' => 'item',
    '#title' => t('Callback URL'),
    '#markup' => url('commerce_expay/pn', array('absolute' => TRUE)),
    '#description' => t('Setup this URL as Callback URL.')
  );
  // Expay system success redirect URL.
  $form['commerce_expay_redirect_success'] = array(
    '#type' => 'item',
    '#title' => t('URL as succeess_url'),
    '#markup' => url('commerce_expay/pn/success', array('absolute' => TRUE)),
    '#description' => t('Setup this URL as succeess_url.')
  );
  // Expay system waiting redirect URL.
  $form['commerce_expay_redirect_waiting'] = array(
    '#type' => 'item',
    '#title' => t('URL as waiting_url'),
    '#markup' => url('commerce_expay/pn/waiting', array('absolute' => TRUE)),
    '#description' => t('Setup this URL as waiting_url.')
  );
  // Expay system scancel redirect URL.
  $form['commerce_expay_redirect_cancel'] = array(
    '#type' => 'item',
    '#title' => t('URL as reject_url'),
    '#markup' => url('commerce_expay/pn/cancel', array('absolute' => TRUE)),
    '#description' => t('Setup this URL as reject_url.')
  );

  $form['#submit'][] = 'commerce_expay_admin_settings_before_submit';

  $form = system_settings_form($form);

  $form['#submit'][] = 'commerce_expay_admin_settings_fetch_submit';

  return $form;
}

/**
 * Administration settings form additional submit
 * for manipulate values before main submit.
 */
function commerce_expay_admin_settings_before_submit($form, &$form_state) {
  $form_state['values']['commerce_expay_merchant_id'] = trim($form_state['values']['commerce_expay_merchant_id']);
  $form_state['values']['commerce_expay_merchant_sk'] = trim($form_state['values']['commerce_expay_merchant_sk']);
}

/**
 * Administration settings form additional submit
 * to fetch methods logos.
 */
function commerce_expay_admin_settings_fetch_submit($form, &$form_state) {
  $values = $form_state['values'];
  $logo_reset = FALSE;
  if (!empty($values['commerce_expay_logos_local_only'])) {
    $logo_reset = TRUE;
  }

  $methods = commerce_expay_get_methods(TRUE, $logo_reset);
  commerce_payment_methods_reset();
  entity_defaults_rebuild();
  rules_clear_cache(TRUE);
}
