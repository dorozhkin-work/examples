<?php

namespace Drupal\cs_custom\Plugin\WebformHandler;

use Drupal\Core\Form\FormStateInterface;
use Drupal\webform\Plugin\WebformHandlerBase;
use Drupal\Component\Utility\Html;
use Drupal\webform\WebformSubmissionInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Webform Phone validate handler.
 *
 * @WebformHandler(
 *   id = "cs_custom_phone_validator",
 *   label = @Translation("Phone validation"),
 *   category = @Translation("Settings"),
 *   description = @Translation("Custom Phone validation handler."),
 *   cardinality = \Drupal\webform\Plugin\WebformHandlerInterface::CARDINALITY_SINGLE,
 *   results = \Drupal\webform\Plugin\WebformHandlerInterface::RESULTS_PROCESSED,
 *   submission = \Drupal\webform\Plugin\WebformHandlerInterface::SUBMISSION_OPTIONAL,
 * )
 */
class CustomPhoneWebformHandler extends WebformHandlerBase {

  use StringTranslationTrait;

  public function validateForm(array &$form, FormStateInterface $form_state,
                               WebformSubmissionInterface $webform_submission) {
    $this->validatePhone($form_state);
  }

  public function validatePhone(FormStateInterface $formState) {
    $phone_fields = [
      'phone',
      'phone_number',
    ];

    foreach ($phone_fields as $phone_field) {
      $value = !empty($formState->getValue($phone_field)) ? Html::escape($formState->getValue($phone_field)) : NULL;
      // Skip empty unique fields or arrays (aka #multiple).
      if (empty($value) || is_array($value)) {
        continue;
      }

      $is_valid = !preg_match('/[A-Za-z]+/', $value) ? TRUE : FALSE;

      if (!$is_valid) {
        $message = $this->t('Phone @value is not valid.', [
          '@value' => $value,
        ]);
        $formState->setErrorByName($phone_field, $message);
      }
      else {
        $formState->setValue($phone_field, $value);
      }
    }
  }

}
