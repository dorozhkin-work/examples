<?php

/**
 * @file
 * Considine card form file.
 */

/**
 * Implements hook_form_FORM_ID_alter().
 * @param $form
 * @param $form_state
 */
function considine_card_form_card_entityform_edit_form_alter(&$form, &$form_state) {
  global $user;

  $card = $form['#entity'];
  $card_wrapper = entity_metadata_wrapper('entityform', $card);

  if (arg(0) == 'create-card') {
    // Hide form on create card page.
    $form['#prefix'] = '<div style="display:none;">';
    $form['#suffix'] = '</div>';
    // By default hide all fields on create card form.
    $elements = element_children($form);
    foreach ($elements as $element) {
      if (strpos($element, 'field_') === 0) {
        $form[$element]['#access'] = FALSE;
      }
    }
    // By default hide all fieldgroups on card create form.
    $fieldgroups = &$form['#fieldgroups'];
    foreach ($fieldgroups as $fieldgroup) {
      $fieldgroup->format_type = 'hidden';
    }

    $workflow_page = considine_card_workflow_page($card);

    // Display trigger save card link on last step.
    if (!empty($workflow_page['prev']) && empty($workflow_page['next'])) {
      $form['workflow_buttons'] = array(
        '#markup' => considine_card_next_button(array('page' => $workflow_page, 'saved' => FALSE))
      );
      //$form['actions']['submit']['#disabled'] = TRUE;
      // Trigger link.
      $trigger_content = '<a id="card-save-link" href="#" popup-opener="#card-save-popup" >popup_opener</a>';
      $form['card_save_trigger'] = array(
        '#markup' => $trigger_content
      );
      // If user not ligged in.
      if (empty($user->uid)) {
        $form['#attached']['js'][] = drupal_get_path('theme', 'considine') . '/js/confirm.js';
      }
    }
    else {
      // Display workflow buttons.
      $form['workflow_buttons'] = array(
        '#markup' => considine_card_next_button(array('page' => $workflow_page))
      );
    }
  }

  $step_type = '';
  $step = $card_wrapper->field_card_step->value();
  if (!empty($step)) {
    $step_type = $step->type;
  }
  // Options for Type steps.
  if ($step_type == 'cstep_type') {
    if (!empty($form['field_card_type'])) {
      $form['field_card_type']['#access'] = TRUE;
      $form['field_card_type'][LANGUAGE_NONE]['#attributes']['ng-model'] = 'cardTypes[activeItem].option_id';
    }
    if (!empty($form['field_card_quantity'])) {
      $form['field_card_quantity']['#access'] = TRUE;
      $form['field_card_quantity'][LANGUAGE_NONE][0]['value']['#attributes']['ng-model'] = 'priceCounter.selected.quantity';
    }
  }
  // Options for Words steps.
  if ($step_type == 'cstep_words') {
    if (!empty($form['field_card_title'])) {
      $form['field_card_title']['#access'] = TRUE;
      $form['field_card_title'][LANGUAGE_NONE][0]['#attributes']['ng-model'] = 'cardTitle';
      $form['field_card_title'][LANGUAGE_NONE][0]['#format'] = 'card';
    }
    if (!empty($form['field_card_body'])) {
      $form['field_card_body']['#access'] = TRUE;
      $form['field_card_body'][LANGUAGE_NONE][0]['#attributes']['ng-model'] = 'cardContent';
      $form['field_card_body'][LANGUAGE_NONE][0]['#format'] = 'card';
    }
  }
  // Options for Paper steps.
  if ($step_type == 'cstep_paper') {
    $flip = considine_card_step_flip($card);
    $flip_enable = variable_get('considine_card_flip_enable', FALSE);
    if ($flip && $flip_enable) {
      if (!empty($form['field_card_back'])) {
        $form['field_card_back']['#access'] = TRUE;
        $form['field_card_back'][LANGUAGE_NONE]['#attributes']['ng-model'] = 'activeColor.back_id';
      }
    }
    else {
      if (!empty($form['field_card_paper_option'])) {
        $form['field_card_paper_option']['#access'] = TRUE;
        $form['field_card_paper_option'][LANGUAGE_NONE]['#attributes']['ng-model'] = 'activeOption.option_id';
      }
      if (!empty($form['field_card_paper'])) {
        $form['field_card_paper']['#access'] = TRUE;
        $form['field_card_paper'][LANGUAGE_NONE]['#attributes']['ng-model'] = 'activeColor.paper_id';
      }
    }
  }
  // Options for Font face steps.
  if ($step_type == 'cstep_font') {
    if (!empty($form['field_card_font'])) {
      $form['field_card_font']['#access'] = TRUE;
      $form['field_card_font'][LANGUAGE_NONE]['#attributes']['ng-model'] = 'activeItem';
    }
    if (!empty($form['field_card_font_color'])) {
      $form['field_card_font_color']['#access'] = TRUE;
      $form['field_card_font_color'][LANGUAGE_NONE]['#attributes']['ng-model'] = 'activeColor.color_id';
    }
  }
  // Options for Border steps.
  if ($step_type == 'cstep_border') {
    if (!empty($form['field_card_border'])) {
      $form['field_card_border']['#access'] = TRUE;
      $form['field_card_border'][LANGUAGE_NONE]['#attributes']['ng-model'] = 'activeColor';
    }
    if (!empty($form['field_card_border_option'])) {
      $form['field_card_border_option']['#access'] = TRUE;
      $form['field_card_border_option'][LANGUAGE_NONE]['#attributes']['ng-model'] = 'activeItem';
    }
  }
  // Options for Motif steps.
  if ($step_type == 'cstep_motif') {
    if (!empty($form['field_card_motif'])) {
      $form['field_card_motif']['#access'] = TRUE;
      $form['field_card_motif'][LANGUAGE_NONE]['#attributes']['ng-model'] = "activeMotif.motif_id";
    }
    if (!empty($form['field_card_motif_option'])) {
      $form['field_card_motif_option']['#access'] = TRUE;
      $form['field_card_motif_option'][LANGUAGE_NONE]['#attributes']['ng-model'] = 'activeMotif.option_id';
    }
  }
  // Options for Motif steps.
  if ($step_type == 'cstep_envelope') {
    if (!empty($form['field_card_envelope'])) {
      $form['field_card_envelope']['#access'] = TRUE;
      $form['field_card_envelope'][LANGUAGE_NONE]['#attributes']['ng-model'] = "envelopes[activeItem].envelope_id";
    }
    if (!empty($form['field_card_envelope_option'])) {
      $form['field_card_envelope_option']['#access'] = TRUE;
      $form['field_card_envelope_option'][LANGUAGE_NONE]['#attributes']['ng-model'] = "envelopes[activeItem].option_id";
    }
    if (!empty($form['field_card_envelope_quantity'])) {
      $form['field_card_envelope_quantity']['#access'] = TRUE;
      $form['field_card_envelope_quantity'][LANGUAGE_NONE][0]['value']['#attributes']['ng-model'] = 'priceCounter.selected.quantity';
    }
  }
  // Button options.
  if (!empty($form['actions'])) {
    // Next button.
    if (!empty($form['actions']['submit'])) {
      $form['actions']['submit']['#id'] = 'card-workflow-submit-next';
      $form['actions']['submit']['#submit'][] = 'considine_card_form_submit_next';
      $form['actions']['submit']['#submit'][] = 'considine_card_form_submit';
    }
    // Prev button.
    $form['actions']['prev'] = array(
      '#type' => 'submit',
      '#value' => t('Prev'),
      '#id' => 'card-workflow-submit-prev',
      '#submit' => array(
        'entityform_edit_form_submit',
        'considine_card_form_submit_prev',
        'considine_card_form_submit'
      ),
      '#entityform_after_review' => FALSE
      //'#limit_validation_errors' => array()
    );
  }
  $form['#submit'] = array();
}

/**
 * Submit function for card form next button.
 * @param $form
 * @param $form_state
 */
function considine_card_form_submit_next($form, &$form_state) {
  // Getting card object.
  $card = &$form_state['entityform'];
  considine_card_procced_next($card);
  $form_state['redirect'] = 'create-card/' . $card->entityform_id;
  drupal_get_messages();
}

/**
 * Submit function for card form prev button.
 * @param $form
 * @param $form_state
 */
function considine_card_form_submit_prev($form, &$form_state) {
  // Getting card object.
  $card = &$form_state['entityform'];
  $card_loaded = entityform_load($card->entityform_id);
  considine_card_procced_prev($card_loaded);
  drupal_get_messages();
  $form_state['entityform'] = $card_loaded;
  $form_state['redirect'] = 'create-card/' . $card->entityform_id;
}

/**
 * Submit function for card form.
 * @param $form
 * @param $form_state
 */
function considine_card_form_submit($form, &$form_state) {
  global $user;
  // Getting card object.
  $card = &$form_state['entityform'];
  $card_wrapper = entity_metadata_wrapper('entityform', $card);

  considine_card_recalculate_price($card);

  $status  = $card_wrapper->field_card_status->value();
  // If card workflow is not finished - redirect on workflow url.
  if ($status != CONSIDINE_CARD_STATUS_PROCESSING) {
    $form_state['redirect'] = 'create-card/' . $card_wrapper->entityform_id->value();
    $card_wrapper->save();
  }
  else {
    // TODO: ADD to cart logic here.
    considine_card_session_delete($card->entityform_id);

    considine_card_add_to_cart($card);

    $form_state['redirect'] = 'basket';

    $card_wrapper->save();
  }
  drupal_get_messages();
}

/**
 * Sets next step and status.
 */
function considine_card_procced_next($card) {
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $current_step = $card_wrapper->field_card_step->value();
  $steps = array_values(considine_card_steps($card));
  foreach ($steps as $key => $step) {
    if ($current_step->nid == $step->nid) {
      if (!empty($steps[$key + 1])) {
        $next_step = $steps[$key + 1];
        $card_wrapper->field_card_step = $next_step;
        $card_wrapper->field_card_status = CONSIDINE_CARD_STATUS_WORKFLOW;
      }
      else {
        // If there is no next step - update status.
        // Set no step.
        $card_wrapper->field_card_status = CONSIDINE_CARD_STATUS_PROCESSING;
      }
    }
  }
}

/**
 * Sets next step and status.
 */
function considine_card_procced_prev($card) {
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $current_step = $card_wrapper->field_card_step->value();
  $steps = array_values(considine_card_steps($card));
  foreach ($steps as $key => $step) {
    // Find our curent step.
    if ($current_step->nid == $step->nid) {
      if (!empty($steps[$key - 1])) {
        $prev_step = $steps[$key - 1];
        // If there is prev step - set it.
        $card_wrapper->field_card_step = $prev_step;
        $card_wrapper->field_card_status = CONSIDINE_CARD_STATUS_WORKFLOW;
      }
    }
  }
  $card_wrapper->save();
}

/**
 * Form to display save card form.
 * Login and register form elements include for UID = 0 users.
 */
function considine_card_save_name_form($form, &$form_state, $card) {
  global $user;

  $form['#id'] = 'considine-card-save-form';

  $form['#theme'] = array('considine_card_save_name_form');

  $form['#attributes'] = array(
    'class' => array('popup-form')
  );

  $form_state['card'] = $card;

  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $name = $card_wrapper->field_card_name->value();
  $form_state['entityform'] = $card;

  $form['card_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Give your stationery a memorable name'),
    '#weight' => -100,
    '#required' => TRUE,
    '#default_value' => (!empty($name)) ? $name : t('Correspondence card')
  );

  if (empty($user->uid)) {

    $form['#validate'] = array('considine_card_save_name_form_pre_validate');
    $form['#submit'] = array('considine_card_save_name_form_submit');

    $register_form = drupal_get_form('user_register_form');
    $form['#user'] = $register_form['#user'];
    $form['#user_category'] = 'register';
    $form['account'] = array(
      '#type' => 'container',
      '#tree' => TRUE
    );
    $form['account']['name'] = array(
      '#type' => 'hidden',
      '#value' => $register_form['account']['name']['#value'],
    );
    $form['account']['mail'] = array(
      '#type' => 'textfield',
      '#title' => $register_form['account']['mail']['#title'],
      '#attributes' => array('class' => array('copy_prohibition'))
    );
    $form['account']['conf_mail'] = array(
      '#type' => 'textfield',
      '#title' => $register_form['account']['conf_mail']['#title'],
      '#attributes' => array('class' => array('copy_prohibition'))
    );
    $form['account']['pass'] = array(
      '#type' => 'password',
      '#title' => $register_form['account']['pass']['#title'],
      '#attributes' => array('class' => array('copy_prohibition'))
    );
    $form['account']['confirm_password'] = array(
      '#type' => 'password',
      '#title' => $register_form['account']['confirm_password']['#title'],
      '#attributes' => array('class' => array('copy_prohibition'))
    );
    $form['account']['buttons'] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('form-row'))
    );
    $form['account']['buttons']['register_submit'] = array(
      '#type' => 'submit',
      '#value' => t('Create Account'), //dont forget to change $op in validators
      '#validate' =>  $form['#validate'],
      '#ajax' => array(
        'callback' => 'considine_site_user_forms_ajax',
        'wrapper' => $form['#id']
      )
    );
    // Setting validate fucntions.
    // IFE validation must always be the last.
    foreach ($register_form['#validate'] as $function) {
      if ($function == 'ife_form_validator') {
        $form['account']['buttons']['register_submit']['#validate'][] = 'considine_card_save_name_form_post_validate';
      }
      $form['account']['buttons']['register_submit']['#validate'][] = $function;
    }
    // Adding validate function to clear messages
    // and make some card name validation.
    $form['account']['buttons']['register_submit']['#validate'][] = 'considine_card_save_name_form_validate';
    // Setting submit functions
    foreach ($register_form['#submit'] as $function) {
      $form['account']['buttons']['register_submit']['#submit'][] = $function;
    }
    $form['account']['buttons']['register_submit']['#submit'][] = 'considine_card_save_name_form_submit';
    $form['account']['buttons']['register_submit']['#submit'][] = 'considine_card_form_submit_next';
    $form['account']['buttons']['register_submit']['#submit'][] = 'considine_card_form_submit';


    // Getting original login form array.
    $login_form = drupal_get_form('user_login');
    $form['login'] = array(
      '#type' => 'container',
      '#tree' => TRUE
    );
    $form['login']['name'] = array(
      '#type' => 'textfield',
      '#title' => $login_form['name']['#title'],
    );
    $form['login']['pass'] = array(
      '#type' => 'password',
      '#title' => $login_form['pass']['#title'],
    );
    $form['login']['buttons'] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('form-row'))
    );
    $form['login']['buttons']['login_submit'] = array(
      '#type' => 'submit',
      '#value' => t('Log in'), //dont forget to change $op in validators
      '#validate' => $form['#validate'],
      '#ajax' => array(
        'callback' => 'considine_site_user_forms_ajax',
        'wrapper' => $form['#id']
      )
    );
    $form['login']['buttons']['links'] = array(
      '#markup' => l('Forgot password?', 'user/password', array('attributes' => array('class' => array('forgot-btn'), 'popup-opener' => '#password-popup'))),
      '#weight' => -10
    );
    // IFE validation must always be the last.
    foreach ($login_form['#validate'] as $key => $function) {
      if ($function == 'ife_form_validator') {
        $form['login']['buttons']['login_submit']['#validate'][] = 'considine_card_save_name_form_post_validate';
      }
      $form['login']['buttons']['login_submit']['#validate'][] = $function;
    }
    // Adding validate function to clear messages
    // and make some card name validation.
    $form['login']['buttons']['login_submit']['#validate'][] = 'considine_card_save_name_form_validate';
    // Setting submit functions
    foreach ($login_form['#submit'] as $function) {
      $form['login']['buttons']['login_submit']['#submit'][] = $function;
    }
    $form['login']['buttons']['login_submit']['#submit'][] = 'considine_card_save_name_form_submit';
    $form['login']['buttons']['login_submit']['#submit'][] = 'considine_card_form_submit_next';
    $form['login']['buttons']['login_submit']['#submit'][] = 'considine_card_form_submit';
  }
  else {
    $form['buttons'] = array(
      '#type' => 'container',
      '#attributes' => array('class' => array('form-row'))
    );
    $form['buttons']['save'] = array(
      '#type' => 'submit',
      '#value' => t('Save card'),
      '#attributes' => array('class' => array('save-button')),
      '#ajax' => array(
        'callback' => 'considine_site_user_forms_ajax',
        'wrapper' => $form['#id']
      ),
      '#submit' => array(
        'considine_card_save_name_form_submit',
        'considine_card_form_submit_next',
        'considine_card_form_submit'
      )
    );
  }
  return $form;
}

/**
 * Validate function to clear all not used drupal messages.
 * Need to be placed after all another validators.
 * Can be used to set some card name validation.
 * @param $form
 * @param $form_state
 */
function considine_card_save_name_form_validate($form, &$form_state) {
  drupal_get_messages(NULL, TRUE);
}

/**
 * Card save name form postvalidate function.
 * Sets correct errors for current form structure based on standart validate results.
 * For more info look into user_register and user_login forms.
 * @param $form
 * @param $form_state
 */
function considine_card_save_name_form_post_validate($form, &$form_state) {
  $errors = form_get_errors();
  $op = drupal_strtolower($form_state['values']['op']);
  // Replace errors with correct element path.
  if ($op == 'login' || $op == 'log in') {
    if (!empty($errors['name'])) {
      form_set_error('login][name', $errors['name']);
    }
    if (!empty($errors['pass'])) {
      form_set_error('login][pass', $errors['pass']);
    }
  }
  if ($op == 'register' || $op == 'create account') {
    if (!empty($errors['mail'])) {
      form_set_error('account][mail', $errors['mail']);
    }
    if (!empty($errors['conf_mail'])) {
      form_set_error('account][conf_mail', $errors['conf_mail']);
    }
    if (!empty($errors['pass'])) {
      form_set_error('account][pass', $errors['pass']);
    }
    if (!empty($errors['confirm_password'])) {
      form_set_error('account][confirm_password', $errors['confirm_password']);
    }
  }
}

/**
 * Card save name form prevalidate function.
 * Sets correct form_state values for register/login standart form proceed.
 * Sets some element_validate functions.
 * For more info look into user_register and user_login forms.
 * @param $form
 * @param $form_state
 */
function considine_card_save_name_form_pre_validate($form, &$form_state) {
  $op = drupal_strtolower($form_state['values']['op']);
  // Need to emulate standart login/register submit form_state
  // for login and register user forms.
  if ($op == 'login' || $op == 'log in') {
    foreach ($form_state['values']['login'] as $key => $value) {
      $form_state['values'][$key] = $value;
      $form_state['input'][$key] = $value;
    }
    // Seeting element_validate functions here.
    email_registration_user_login_validate($form, $form_state);
  }
  if ($op == 'register' || $op == 'create account') {
    foreach ($form_state['values']['account'] as $key => $value) {
      $form_state['values'][$key] = $value;
      $form_state['input'][$key] = $value;
    }
    // Seeting element_validate functions here.
    considine_confirm_password_validate($form['account']['confirm_password'], $form_state);
  }

}

/**
 * Card save name submit.
 * @param $form
 * @param $form_state
 */
function considine_card_save_name_form_submit($form, &$form_state) {
  global $user;
  $values = $form_state['values'];
  $card_name = $values['card_name'];
  $card = $form_state['card'];
  $card->uid = $user->uid;
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $card_wrapper->field_card_name = $card_name;
  $card_wrapper->save();
  $form_state['entityform'] = $card_wrapper->value();
  if (!empty($values['account'])) {
    $form_state['redirect'] = 'create-card/' . $card->entityform_id;
  }
  else {
    $form_state['redirect'] = 'basket';
  }
}


/**
 * Card workflow form.
 * @param  $card
 *   Card object (entityform).
 */
function considine_card_workflow_form($form, &$form_state, $card) {

  $form_state['card'] = $card;

  $card_wrapper = entity_metadata_wrapper('entityform', $card);

  // Getting current step.
  $step = $card_wrapper->field_card_step->value();

  if (!empty($step)) {
    // Getting step options.
    $options = array();
    $option_fn = "considine_card_" . $step->type . "_options";
    if (function_exists($option_fn)) {
      // Options configuration.
      $options = $option_fn($step, $card);
    }

    if (!empty($options)) {
      $form['#attached']['js'][] = array(
        'data' => 'var ' . $options['name'] . '=' . drupal_json_encode($options['options']) . ';',
        'type' => 'inline',
        'scope' => 'header',
        'group' => JS_LIBRARY,
        'weight' => -10
      );
    }

    // Setting current step elements.
    $element_fn = "considine_card_workflow_" . $step->type . "_form";
    if (function_exists($element_fn)) {
      $form += $element_fn($card, $step, $options);
    }
    // Setting current step submit function.
    $submit_fn = "considine_card_workflow_" . $step->type . "_form_submit";
    if (function_exists($submit_fn)) {
      $form['#submit'][] = $submit_fn;
    }
  }

  // Hide form.
  // $form['#prefix'] = '<div style="display:none;">';
  // $form['#suffix'] = '</div>';

  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#id' => 'card-workflow-submit'
  );

  // Setting default submit and validate functions.
  $form['#validate'][] = 'considine_card_workflow_form_validate';
  $form['#submit'][] = 'considine_card_workflow_form_submit';
  return $form;
}

/**
 * Default validate function for considine_card_workflow_form.
 * @param $form
 * @param $form_state
 */
function considine_card_workflow_form_validate($form, &$form_state) {}

/**
 * Default submit function for considine_card_workflow_form.
 * @param $form
 * @param $form_state
 */
function considine_card_workflow_form_submit($form, &$form_state) {
  global $user;
  // Getting card object.
  $card = &$form_state['card'];
  $card_wrapper = entity_metadata_wrapper('entityform', $card);

  considine_card_procced_next($card);

  $status  = $card_wrapper->field_card_status->value();
  // If card workflow is not finished - redirect on workflow url.
  if ($status != CONSIDINE_CARD_STATUS_PROCESSING) {
    $form_state['redirect'] = 'create-card/' . $card_wrapper->entityform_id->value();
    $card_wrapper->save();
  }
  else {
    // TODO: ADD to cart logic here.
    considine_card_session_delete($card->entityform_id);

    considine_card_add_to_cart($card);

    $form_state['redirect'] = 'basket/' . commerce_cart_order_id($user->uid);

    $card_wrapper->save();
  }
}

/**
 * Card form element for "Card type" step
 * @param $card
 */
function considine_card_workflow_cstep_type_form($card, $step, $options = array()) {
  $elements = array();

  // Getting options list.
  $list = array();
  $default_value = array();
  if (!empty($options['options'])) {
    foreach ($options['options'] as $key => $option) {
      $list[$key] = $option['title'];
      // Getting default value.
      if (!empty($option['active']) && $option['active'] == 'active') {
        $default_value = $key;
      }
    }
  }


  $elements['card_type'] = array(
    '#type' => 'select',
    '#title' => t('Card type'),
    '#options' => $list,
    '#default_value' => ($default_value) ? $default_value : FALSE,
    '#attributes' => array('ng-model' => 'activeItem')
  );
  return $elements;
}

/**
 * Submit function for "Card type" step.
 * @param $form
 * @param $form_state
 */
function considine_card_workflow_cstep_type_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $type_id = $values['card_type'];
  if (!empty($type_id)) {
    $card = &$form_state['card'];
    $card_wrapper = entity_metadata_wrapper('entityform', $card);
    $card_wrapper->field_card_type = $type_id;
    $card_wrapper->save();
  }
}


/**
 * Card form element for "Card words" step
 * @param $card
 */
function considine_card_workflow_cstep_words_form($card, $step, $options = array()) {
  $elements = array();
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  // Text 1 element.
  $text1_default = $card_wrapper->field_card_title->value();
  $elements['card_text1'] = array(
    '#type' => 'textarea',
    '#title' => t('Card Text 1'),
    '#default_value' => ($text1_default) ? $text1_default : false,
    '#attributes' => array('ng-model' => 'cardTitle')
  );
  // Text 2 element.
  $text2_default = $card_wrapper->field_card_body->value();
  $elements['card_text2'] = array(
    '#type' => 'textarea',
    '#title' => t('Card Text 2'),
    '#default_value' => ($text2_default) ? $text2_default : false,
    '#attributes' => array('ng-model' => 'cardContent')
  );
  return $elements;
}

/**
 * Submit function for "Card type" step.
 * @param $form
 * @param $form_state
 */
function considine_card_workflow_cstep_words_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $text1 = $values['card_text1'];
  $text2 = $values['card_text2'];
  $format = 'card';

  $card = &$form_state['card'];
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $card_wrapper->field_card_title->set(array('value' => $text1, 'format' => $format));
  $card_wrapper->field_card_body->set(array('value' => $text2, 'format' => $format));
  $card_wrapper->save();
}

/**
 * Card form element for "Card paper" step
 * @param $card
 */
function considine_card_workflow_cstep_paper_form($card, $step, $options = array()) {
  $elements = array();
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  // Getting paper IDs.
  // $paper_option_id = $card_wrapper->field_card_paper_option->raw();
  // $paper_id = $card_wrapper->fild_card_paper->raw();

  $paper_options_list = array();
  $paper_list = array();
  $default_paper_option = 0;
  $default_paper = 0;
  if (!empty($options['options'])) {
    foreach ($options['options'] as $option_id => $option) {
      $paper_options_list[$option_id] = $option['title'];
      if ($option['active'] == 'active') {
        $default_paper_option = $option_id;
      }
      if (!empty($option['colors'])) {
        foreach ($option['colors'] as $color_id => $color) {
          $paper_list[$color_id] = $color['name'];
          if ($color['active'] == 'active') {
            $default_paper = $color_id;
          }
        }
      }
    }
  }

  $elements['card_paper_option'] = array(
    '#type' => 'select',
    '#title' => t('Card paper option'),
    '#default_value' => ($default_paper_option) ? $default_paper_option : false,
    '#attributes' => array('ng-model' => 'cardTitle'),
    '#options' => $paper_options_list
  );
  $elements['card_paper'] = array(
    '#type' => 'select',
    '#title' => t('Card paper'),
    '#default_value' => ($default_paper) ? $default_paper : false,
    '#attributes' => array('ng-model' => 'cardTitle'),
    '#options' => $paper_list
  );
  return $elements;
}

/**
 * Submit function for "Card type" step.
 * @param $form
 * @param $form_state
 */
function considine_card_workflow_cstep_paper_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $text1 = $values['card_text1'];
  $text2 = $values['card_text2'];
  $format = 'card';

  $card = &$form_state['card'];
  $card_wrapper = entity_metadata_wrapper('entityform', $card);
  $card_wrapper->field_card_title->set(array('value' => $text1, 'format' => $format));
  $card_wrapper->field_card_body->set(array('value' => $text2, 'format' => $format));
  $card_wrapper->save();
}
