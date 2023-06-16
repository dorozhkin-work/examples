<?php

namespace Drupal\currency_minimal_rate;

use Drupal\Core\Database\Connection;

/**
 * Service description.
 */
class CurrencyMinimalRateDatabaseStorage implements CurrencyMinimalRateStorageInterface {

  /**
   * The database connection.
   *
   * @var \Drupal\Core\Database\Connection
   */
  protected $connection;

  /**
   * Constructs a CurrencyMinimalRateStorage object.
   *
   * @param \Drupal\Core\Database\Connection $connection
   *   The database connection.
   */
  public function __construct(Connection $connection) {
    $this->connection = $connection;
  }

  /**
   * {@inheritdoc}
   */
  public function saveRate($currency_code, $number) {
    return (bool) $this->connection
      ->merge('currency_minimal_rate')
      ->key('currency_code', $currency_code)
      ->fields([
        'number' => $number,
      ])
      ->execute();
  }

  /**
   * {@inheritdoc}
   */
  public function saveRates($rates) {
    if (empty($rates)) {
      return FALSE;
    }
    $query = $this->connection->insert('currency_minimal_rate');
    $query->fields(['currency_code', 'number']);
    foreach ($rates as $code => $value) {
      $query->values([
        'currency_code' => $code,
        'number' => $value,
      ]);
    }
    return (bool) $query->execute();
  }

  /**
   * {@inheritdoc}
   */
  public function fetchRates($currency_codes = []) {
    $rates = [];
    $query = $this->connection->select('currency_minimal_rate', 'cmr');
    $query->fields('cmr', ['currency_code', 'number']);
    if (!empty($currency_codes)) {
      $query->condition('currency_code', $currency_codes, 'IN');
    }
    $data = $query->execute()->fetchAll();
    foreach ($data as $rate) {
      $rates[$rate->currency_code] = new CurrencyMinimalRateResult($rate->currency_code, $rate->number);
    }
    return $rates;
  }

  /**
   * {@inheritdoc}
   */
  public function fetchRate($currency_code) {
    $rates = $this->fetchRates([$currency_code]);
    return reset($rates);
  }

  /**
   * {@inheritdoc}
   */
  public function cleanup() {
    return (bool) $this->connection
      ->delete('currency_minimal_rate')
      ->execute();
  }

}
