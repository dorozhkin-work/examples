<?php

/**
 * @file
 * Implements new rules events actions and conditions.
 */

/**
 * Implements hook_rules_event_info().
 */
function commerce_expay_rules_event_info() {
  $events = array();

  $events['commerce_expay_transaction_callback_update'] = array(
    'label' => t('After transaction updated on Expay callback.'),
    'group' => t('Commerce Expay'),
    'variables' => array(
      'expay_method' => array(
        'label' => t('Expay callback method'),
        'type' => 'text',
      ),
      'commerce_payment_transaction' => array(
        'label' => t('Comemrce payment transaction'),
        'type' => 'commerce_payment_transaction',
      ),
    ),
    'access callback' => 'commerce_order_rules_access',
  );

  $events['commerce_expay_transaction_status_update'] = array(
    'label' => t('After transaction updated via getStatus.'),
    'group' => t('Commerce Expay'),
    'variables' => array(
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
      ),
      'commerce_payment_transaction' => array(
        'label' => t('Comemrce payment transaction'),
        'type' => 'commerce_payment_transaction',
      ),
      'expay_payment_type' => array(
        'label' => t('Expay payment type'),
        'type' => 'text',
        'description' => t('Can be two values: online and offline.'),
        'options list' => 'commerce_expay_rules_remote_method_types',
      ),
      'expay_status' => array(
        'label' => t('Expay remote status'),
        'type' => 'text',
      ),
    ),
    'access callback' => 'commerce_order_rules_access',
  );

  return $events;
}

/**
 * Return a list Expay method types.
 *
 * @return array
 */
function commerce_expay_rules_remote_method_types() {
  $list = array(
    'offline' => 'Offline',
    'online' => 'Online'
  );
  return $list;
}

/**
 * Implements hook_rules_action_info().
 */
function commerce_expay_rules_action_info() {
  $actions = array();

  $actions['commerce_expay_update_order_transactions'] = array(
    'label' => t('Update order transactions statuses by calling getStatus.'),
    'parameter' => array(
      'order' => array(
        'type' => 'commerce_order',
        'label' => t('Order'),
      ),
      'status' => array(
        'type' => 'text',
        'label' => t('Transactions with status to check'),
        'options list' => 'commerce_expay_rules_transaction_status_list',
      ),
    ),
    'callbacks' => array(
      'execute' => 'commerce_expay_update_order_transactions',
    ),
    'group' => t('Commerce Expay'),
  );

  return $actions;
}

/**
 * Return a list commerce payment transaction statuses.
 *
 * @return array
 */
function commerce_expay_rules_transaction_status_list() {
  $statuses = commerce_payment_transaction_statuses();
  $list = array();
  foreach ($statuses as $status) {
    $list[$status['status']] = $status['title'];
  }
  return $list;
}

/**
 * Updates order transactions by calling expay getStatus method.
 *
 * @param $order
 * @param $status
 */
function commerce_expay_update_order_transactions($order, $status) {
  $options = array(
    'order_id' => $order->order_id,
    'status' => $status,
    'payment_method' => 'commerce_expay'
  );

  $transactions = commerce_payment_transaction_load_multiple(array(), $options);
  $geteway_status = TRUE;
  if (!empty($transactions)) {
    foreach ($transactions as $transaction) {
      $payload = $transaction->payload;
      if (empty($payload['expay_payment'])) {
        continue;
      }
      $expay_payment = $payload['expay_payment'];
      $remote_payment_order = $expay_payment['order'];
      $geteway_status = commerce_expay_transaction_status_update($transaction, $remote_payment_order, $force_save = TRUE);
    }
  }
  if (!$geteway_status) {
    drupal_set_message(t('During update statuses of orders from gateway Expay has been errors'), 'error');
  }
}
