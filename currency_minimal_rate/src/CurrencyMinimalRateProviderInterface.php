<?php

namespace Drupal\currency_minimal_rate;

/**
 * Interface for currency_minimal_rate_provider plugins.
 */
interface CurrencyMinimalRateProviderInterface {

  /**
   * Returns the translated plugin label.
   *
   * @return string
   *   The translated title.
   */
  public function label();

  /**
   * URL for external endpoint.
   *
   * @return string
   *   Full url path.
   */
  public function endpointUrl();

  /**
   * Generic wrapper around Drupal http client.
   *
   * @param array $options
   *   Additional request options.
   *
   * @return mixed
   *   Return response, or error.
   */
  public function endpointClient(array $options);

  /**
   * Method which supports remote endpoint.
   *
   * @return string
   *   GET or POST.
   */
  public function endpointMethod();

  /**
   * Method to load data from remote endpoint.
   *
   * @return \Drupal\currency_minimal_rate\CurrencyMinimalRateResult[]
   *   Return result data list.
   */
  public function loadRemoteData();

}
