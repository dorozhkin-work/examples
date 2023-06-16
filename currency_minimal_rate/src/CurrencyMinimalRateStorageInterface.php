<?php

namespace Drupal\currency_minimal_rate;

/**
 * Provides an interface defining Currency minimal rate storage.
 *
 * Stores minimal currency rate number, timestamp of last update.
 */
interface CurrencyMinimalRateStorageInterface {

  /**
   * Saves currency rate.
   *
   * @param string $code
   *   Currency code unique string.
   * @param string $number
   *   Currency rate number decimal string.
   *
   * @return bool
   *   TRUE if currency rate record was saved.
   */
  public function saveRate($code, $number);

  /**
   * Saves currency rates list.
   *
   * @param array $rates
   *   List of rates to save.
   *
   * @return bool
   *   TRUE if rates records was saved.
   */
  public function saveRates($rates);

  /**
   * Returns specific currency rate value object from database.
   *
   * @param string $code
   *   The specific currency code to search for.
   *
   * @return \Drupal\currency_minimal_rate\CurrencyMinimalRateResult|false
   *   If record exists, a value object returned.
   *   If it does not exist, FALSE will be returned.
   */
  public function fetchRate($code);

  /**
   * Returns a list of currency rates value objects from database.
   *
   * @param array $codes
   *   An array of currency codes to fetch from databases.
   *
   * @return \Drupal\currency_minimal_rate\CurrencyMinimalRateResult[]
   *   An array of value objects representing currency rates.
   */
  public function fetchRates($codes = []);

  /**
   * Deletes rates records.
   *
   * @return bool
   *   True if cleanup was successful, False otherwise.
   */
  public function cleanup();

}
