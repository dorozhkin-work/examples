<?php

namespace Drupal\currency_minimal_rate;

/**
 * Interface CurrencyMinimalRateManagerInterface.
 *
 * @package Drupal\currency_minimal_rate
 */
interface CurrencyMinimalRateComparatorInterface {

  /**
   * Calls currency providers to fetching data.
   */
  public function execute();

}
