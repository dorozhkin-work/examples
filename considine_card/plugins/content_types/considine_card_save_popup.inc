<?php

/**
 * @file
 * Plugin to handle the Considine remember password form.
 */

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */
$plugin = array(
  'title' => t('Considine card save'),
  'description' => t('Add\'s Considine card save form.'),
  'category' => t('Considine'),
  'render callback' => 'considine_card_save_form_pane_render'
);

/**
 * Output function for the considine copyright pane.
 */
function considine_card_save_form_pane_render($subtype, $conf, $panel_args, $contexts) {
  global $user;
  $block = new stdClass();
  // TODO: add card loading by URL or args.
  $card = NULL;
  if (!empty($contexts->data)) {
    $card = $contexts->data;
  }
  if (empty($card)) {
    $arg1 = arg(0);
    $arg2 = arg(1);
    if ($arg1 == 'create-card' && (!empty($arg2) && is_numeric($arg2))) {
      $card = entityform_load((int)$arg2);
    }
    else {
      $card = considine_card_card_load($user->uid);
    }
  }
  $form = drupal_get_form('considine_card_save_name_form', $card);
  $workflow_page = considine_card_workflow_page($card);
  if (!empty($workflow_page['next'])) {
    $content['title'] = array(
      '#markup' => t('Do you wish to save your work?')
    );
    $subtitle = array(
      t("You have not finished your stationery"),
      t("If you sign in or create an account then you will be able to finish it later"),
      t("Click X to continue creating your stationery and save later"),
      '<a class="link-button link-redirect" href="#">' . t("Don't save it for me") . '</a>'
    );
  }
  else {
    $content['title'] = array(
      '#markup' => t('Your design is now complete')
    );
    $subtitle = array(
      t("That's it. You've created your stationery."),
      t("Before you continue, we need to save what you have done."),
      t("Please log in/create an account so we can save it for you.")
    );
  }
  $content['subtitle'] = array(
    '#markup' => implode('<br>', $subtitle)
  );
  $content['body'] = array('card_save_form' => $form);
  $attributes = array(
    'class' => array('popup', 'form-popup'),
    'id' => 'card-save-popup'
  );
  $block->content = theme('considine_form_popup', array('content' => $content, 'attributes_array' => $attributes));
  return $block;
}
