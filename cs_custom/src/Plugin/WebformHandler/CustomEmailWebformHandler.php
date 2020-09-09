<?php

namespace Drupal\cs_custom\Plugin\WebformHandler;

use Drupal\Core\Form\FormStateInterface;
use Drupal\webform\Plugin\WebformHandlerBase;
use Drupal\Component\Utility\Html;
use Drupal\webform\WebformSubmissionInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Webform email validate handler.
 *
 * @WebformHandler(
 *   id = "cs_custom_email_validator",
 *   label = @Translation("Email validation"),
 *   category = @Translation("Settings"),
 *   description = @Translation("Custom email validation handler."),
 *   cardinality = \Drupal\webform\Plugin\WebformHandlerInterface::CARDINALITY_SINGLE,
 *   results = \Drupal\webform\Plugin\WebformHandlerInterface::RESULTS_PROCESSED,
 *   submission = \Drupal\webform\Plugin\WebformHandlerInterface::SUBMISSION_OPTIONAL,
 * )
 */
class CustomEmailWebformHandler extends WebformHandlerBase {

  use StringTranslationTrait;

  public function validateForm(array &$form, FormStateInterface $form_state,
                               WebformSubmissionInterface $webform_submission) {
    $this->validateEmail($form_state);
  }

  public function validateEmail(FormStateInterface $formState) {
    $value = !empty($formState->getValue('email')) ? Html::escape($formState->getValue('email')) : NULL;
    // Skip empty unique fields or arrays (aka #multiple).
    if (empty($value) || is_array($value)) {
      return;
    }

    $is_valid = \Drupal::service('email.validator')->isValid($value);
    if ($is_valid) {
      $is_valid = filter_var($value, FILTER_VALIDATE_EMAIL);
    }

    if (!$is_valid) {
      $message = $this->t('The email address @value is not valid.', [
        '@value' => $value,
      ]);
      $formState->setErrorByName('email', $message);
    }
    else {
      $formState->setValue('email', $value);
    }
  }

}
