<?php


namespace Drupal\cs_custom\Plugin\WebformHandler;

use Drupal\Core\Form\FormStateInterface;
use Drupal\webform\Plugin\WebformHandlerBase;
use Drupal\Component\Utility\Html;
use Drupal\webform\WebformSubmissionInterface;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Webform First name & Last name validate handler.
 *
 * @WebformHandler(
 *   id = "cs_custom_name_validator",
 *   label = @Translation("Name validation"),
 *   category = @Translation("Settings"),
 *   description = @Translation("Custom First name & Last name validation handler."),
 *   cardinality = \Drupal\webform\Plugin\WebformHandlerInterface::CARDINALITY_SINGLE,
 *   results = \Drupal\webform\Plugin\WebformHandlerInterface::RESULTS_PROCESSED,
 *   submission = \Drupal\webform\Plugin\WebformHandlerInterface::SUBMISSION_OPTIONAL,
 * )
 */
class CustomNameWebformHandler extends WebformHandlerBase {

  use StringTranslationTrait;

  public function validateForm(array &$form,FormStateInterface $form_state,
                               WebformSubmissionInterface $webform_submission) {
    $this->validateName($form_state);
  }

  public function validateName(FormStateInterface $formState) {
    $name_fields = [
      'name',
      'first_name',
      'last_name',
    ];

    foreach ($name_fields as $name_field) {
      $value = !empty($formState->getValue($name_field)) ? $formState->getValue($name_field) : NULL;
      // Skip empty unique fields or arrays (aka #multiple).
      if (empty($value) || is_array($value)) {
        continue;
      }

      $is_valid = $this->nameValidationHandler($value);

      if (!$is_valid) {
        $message = $this->t('Name @value is not valid.', [
          '@value' => $value,
        ]);
        $formState->setErrorByName($name_field, $message);
      }
      else {
        $formState->setValue($name_field, $value);
      }
    }
  }

  /**
   * Validates name (checks for digits and some special chars).
   *
   * @param string $name
   * @return bool
   */
  private function nameValidationHandler($name) {
    $num_pattern = '~[0-9]~';
    $spec_chars_pattern = '/["^£$%&*()}{@#~!;:?><>,|=_+¬]/';

    $has_numbers = preg_match($num_pattern, $name) ? TRUE : FALSE;
    $has_spec_chars = preg_match($spec_chars_pattern, $name) ? TRUE : FALSE;
    $is_valid = !$has_numbers && !$has_spec_chars;

    return $is_valid;
  }

}
