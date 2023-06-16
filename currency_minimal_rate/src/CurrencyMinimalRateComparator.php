<?php

namespace Drupal\currency_minimal_rate;

use Drupal\Core\Cache\Cache;
use Drupal\Core\Config\ConfigFactory;
use Drupal\Core\Logger\LoggerChannelFactoryInterface;
use Drupal\Core\State\StateInterface;

/**
 * Main comparator class.
 */
class CurrencyMinimalRateComparator implements CurrencyMinimalRateComparatorInterface {

  /**
   * Rates list.
   *
   * @var array
   */
  protected $rates;

  /**
   * The state.
   *
   * @var \Drupal\Core\State\StateInterface
   */
  protected $state;

  /**
   * The logger channel.
   *
   * @var \Drupal\Core\Logger\LoggerChannelInterface
   */
  protected $logger;

  /**
   * The providers plugin manager.
   *
   * @var \Drupal\Component\Plugin\PluginManagerInterface
   */
  protected $providersManager;

  /**
   * The storage.
   *
   * @var \Drupal\currency_minimal_rate\CurrencyMinimalRateStorageInterface
   */
  protected $storage;

  /**
   * Config.
   *
   * @var \Drupal\Core\Config\ImmutableConfig
   */
  protected $config;

  /**
   * Debug.
   *
   * @var bool
   */
  protected $debug;

  /**
   * Constructs a CurrencyMinimalRateManager object.
   *
   * @param \Drupal\Core\State\StateInterface $state
   *   The state.
   * @param \Drupal\Core\Logger\LoggerChannelFactoryInterface $logger
   *   The logger channel factory.
   * @param \Drupal\Component\Plugin\PluginManagerInterface $providersManager
   *   The plugin manager.
   * @param \Drupal\currency_minimal_rate\CurrencyMinimalRateStorageInterface $storage
   *   The plugin manager.
   */
  public function __construct(StateInterface $state, LoggerChannelFactoryInterface $logger, CurrencyMinimalRateProviderPluginManager $providersManager, CurrencyMinimalRateStorageInterface $storage, ConfigFactory $config_factory) {
    $this->state = $state;
    $this->logger = $logger->get('currency_minimal_rate');
    $this->providersManager = $providersManager;
    $this->storage = $storage;
    $this->config = $config_factory->get('currency_minimal_rate.settings');
    $this->rates = [];
    $this->debug = $this->config->get('debug');
  }

  /**
   * Gets rates.
   */
  public function getRates() {
    return $this->rates;
  }

  /**
   * Gets providers.
   */
  public function getProviders() {
    return $this->providersManager->getDefinitions();
  }

  /**
   * Gets providers name list.
   */
  public function getProvidersNames() {
    $names = [];
    $providers = $this->getProviders();
    foreach ($providers as $provider_definition) {
      $names[$provider_definition['id']] = $provider_definition['label'];
    }
    return $names;
  }

  /**
   * Processing remote data.
   */
  protected function processRemoteData() {
    // Getting all providers plugins.
    $providers = $this->getProviders();
    // Run all providers plugins.
    foreach ($providers as $provider_definition) {
      // Instantiate provider object.
      $provider = $this->providersManager->createInstance($provider_definition['id']);
      // Loading processed list of rates.
      $rates = $provider->loadRemoteData();
      // Create list of compared rates.
      foreach ($rates as $rate) {
        $rate_code = $rate->getCode();
        $rate_number = $rate->getNumber();
        // Compare to find minimal rate.
        if (isset($this->rates[$rate_code]) && $this->rates[$rate_code] <= $rate_number) {
          continue;
        }
        $this->rates[$rate_code] = $rate_number;
      }
    }
  }

  /**
   * {@inheritDoc}
   */
  public function execute() {
    // Processing remote rates.
    $this->processRemoteData();
    // Store processed rates.
    if ($this->rates) {
      // Cleanup prev records.
      $this->storage->cleanup();
      // Save new rates records.
      $this->storage->saveRates($this->rates);
    }
    $updated = time();
    $updated_formatted = date('d-m-Y H:i:s');
    // Save latest update time.
    $this->state->set('currency_minimal_rate.updated', $updated);
    // Log results.
    $this->logger->info('%count rates were updated at %updated.', [
      '%count' => count($this->rates),
      '%updated' => $updated_formatted,
    ]);
    if ($this->debug) {
      $this->logger->debug('Final list: <pre><code>' . print_r($this->rates, TRUE) . '</code></pre>');
    }
    // Invalidate cache.
    Cache::invalidateTags(['currency_minimal_rate']);
  }

}
