<?php

namespace Drupal\currency_minimal_rate;

use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;
use Drupal\Core\Plugin\DefaultPluginManager;

/**
 * CurrencyMinimalRateProvider plugin manager.
 */
class CurrencyMinimalRateProviderPluginManager extends DefaultPluginManager {

  /**
   * Constructs CurrencyMinimalRateProviderPluginManager object.
   *
   * @param \Traversable $namespaces
   *   An object that implements \Traversable which contains the root paths
   *   keyed by the corresponding namespace to look for plugin implementations.
   * @param \Drupal\Core\Cache\CacheBackendInterface $cache_backend
   *   Cache backend instance to use.
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   *   The module handler to invoke the alter hook with.
   */
  public function __construct(\Traversable $namespaces, CacheBackendInterface $cache_backend, ModuleHandlerInterface $module_handler) {
    parent::__construct(
      'Plugin/CurrencyMinimalRateProvider',
      $namespaces,
      $module_handler,
      'Drupal\currency_minimal_rate\CurrencyMinimalRateProviderInterface',
      'Drupal\currency_minimal_rate\Annotation\CurrencyMinimalRateProvider'
    );
    $this->alterInfo('currency_minimal_rate_provider_info');
    $this->setCacheBackend($cache_backend, 'currency_minimal_rate_provider_plugins');
  }

}
