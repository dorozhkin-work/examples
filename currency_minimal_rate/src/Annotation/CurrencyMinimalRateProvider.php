<?php

namespace Drupal\currency_minimal_rate\Annotation;

use Drupal\Component\Annotation\Plugin;

/**
 * Defines currency_minimal_rate_provider annotation object.
 *
 * @Annotation
 */
class CurrencyMinimalRateProvider extends Plugin {

  /**
   * The plugin ID.
   *
   * @var string
   */
  public $id;

  /**
   * The human-readable name of the plugin.
   *
   * @var \Drupal\Core\Annotation\Translation
   *
   * @ingroup plugin_translatable
   */
  public $label;

}
