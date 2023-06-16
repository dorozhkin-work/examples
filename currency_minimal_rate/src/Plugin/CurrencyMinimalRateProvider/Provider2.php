<?php

namespace Drupal\currency_minimal_rate\Plugin\CurrencyMinimalRateProvider;

use Drupal\Component\Serialization\Json;
use Drupal\currency_minimal_rate\CurrencyMinimalRateProviderPluginBase;
use Drupal\currency_minimal_rate\CurrencyMinimalRateResult;

/**
 * Plugin implementation of the currency_minimal_rate_provider.
 *
 * @CurrencyMinimalRateProvider(
 *   id = "provider2",
 *   label = @Translation("Provider 2")
 * )
 */
class Provider2 extends CurrencyMinimalRateProviderPluginBase {

  /**
   * {@inheritdoc}
   */
  public function endpointUrl() {
    $url = "https://www.mocky.io/v3/01dac20a-0b85-462a-a0c4-6ad38cba7109";
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
      if (!empty($json['result'])) {
        foreach ($json['result'] as $rate) {
          $currency_code = $this->normalizeCurrencyCode($rate['symbol']);
          $result = new CurrencyMinimalRateResult($currency_code, $rate['amount']);
          $data[$result->getCode()] = $result;
        }
      }
    }
    return $data;
  }

  /**
   * {@inheritdoc}
   */
  public function normalizeCurrencyCode(string $code) {
    $code = parent::normalizeCurrencyCode($code);
    return substr($code, 0, 3);
  }

}
