<?php

namespace Drupal\currency_minimal_rate\Plugin\CurrencyMinimalRateProvider;

use Drupal\Component\Serialization\Json;
use Drupal\currency_minimal_rate\CurrencyMinimalRateProviderPluginBase;
use Drupal\currency_minimal_rate\CurrencyMinimalRateResult;

/**
 * Plugin implementation of the currency_minimal_rate_provider.
 *
 * @CurrencyMinimalRateProvider(
 *   id = "provider1",
 *   label = @Translation("Provider 1")
 * )
 */
class Provider1 extends CurrencyMinimalRateProviderPluginBase {

  /**
   * {@inheritdoc}
   */
  public function endpointUrl() {
    $url = "https://www.mocky.io/v3/81c1dc29-bac5-4ee0-a577-4f47432a3206";
    return $url;
  }

  /**
   * {@inheritdoc}
   */
  public function loadRemoteData() {
    $data = [];
    $options = [];
    // Requesting endpoint.
    $request = $this->endpointClient($options);
    // Proceed with response.
    if ($request) {
      $json = Json::decode($request);
      if (!empty($json)) {
        foreach ($json as $rate) {
          $currency_code = $this->normalizeCurrencyCode($rate['currency']);
          $result = new CurrencyMinimalRateResult($currency_code, $rate['value']);
          $data[$result->getCode()] = $result;
        }
      }
    }
    return $data;
  }

  /**
   * {@inheritdoc}
   */
  protected function getCurrencyCodeMap() {
    return [
      'DOLAR' => 'USD',
      'EURO' => 'EUR',
      'POUND' => 'GBP',
    ];
  }

}
