<?php

/**
 * @file
 * Commerce expay default rules.
 */

/**
 * Implements hook_default_rules_configuration().
 */
function commerce_expay_default_rules_configuration() {
  $items = array();

  $items['rules_expay_update_order_transactions'] = entity_import('rules_config', '{ "rules_expay_update_order_transactions" : {
      "LABEL" : "Expay update order transactions",
      "PLUGIN" : "reaction rule",
      "OWNER" : "rules",
      "TAGS" : [ "Expay" ],
      "REQUIRES" : [ "rules", "commerce_expay", "entity" ],
      "ON" : { "commerce_order_view" : [] },
      "IF" : [
        { "NOT data_is" : { "data" : [ "commerce-order:status" ], "value" : "canceled" } }
      ],
      "DO" : [
        { "commerce_expay_update_order_transactions" : { "order" : [ "commerce-order" ], "status" : "pending" } }
      ]
    }
  }');

  $items['rules_expay_cancel_order'] = entity_import('rules_config', '{ "rules_expay_cancel_order" : {
      "LABEL" : "Expay cancel order",
      "PLUGIN" : "reaction rule",
      "OWNER" : "rules",
      "TAGS" : [ "Expay" ],
      "REQUIRES" : [ "rules", "commerce_order", "commerce_expay" ],
      "ON" : { "commerce_expay_transaction_status_update" : [] },
      "IF" : [
        { "data_is" : { "data" : [ "expay-status" ], "value" : "204" } },
        { "data_is" : { "data" : [ "expay-payment-type" ], "value" : "offline" } }
      ],
      "DO" : [
        { "commerce_order_update_status" : { "commerce_order" : [ "order" ], "order_status" : "canceled" } }
      ]
    }
  }');

  $items['rules_expay_new_order'] = entity_import('rules_config', '{ "rules_expay_new_order" : {
      "LABEL" : "Expay new order",
      "PLUGIN" : "reaction rule",
      "OWNER" : "rules",
      "TAGS" : [ "Expay" ],
      "REQUIRES" : [ "rules", "commerce_payment", "commerce_order", "entity" ],
      "ON" : { "commerce_order_update" : [] },
      "IF" : [
        { "data_is" : {
            "data" : [ "commerce-order:state" ],
            "op" : "IN",
            "value" : { "value" : { "cart" : "cart", "checkout" : "checkout" } }
          }
        },
        { "data_is" : {
            "data" : [ "commerce-order-unchanged:state" ],
            "op" : "IN",
            "value" : { "value" : { "cart" : "cart", "checkout" : "checkout" } }
          }
        },
        { "commerce_payment_order_balance_comparison" : {
            "commerce_order" : [ "commerce_order" ],
            "operator" : "=",
            "value" : "0"
          }
        }
      ],
      "DO" : [
        { "commerce_order_update_status" : { "commerce_order" : [ "commerce_order" ], "order_status" : "pending" } },
        { "redirect" : { "url" : "\u003Cfront\u003E" } }
      ]
    }
  }');


  return $items;
}
