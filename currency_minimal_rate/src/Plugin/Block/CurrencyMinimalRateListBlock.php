<?php

namespace Drupal\currency_minimal_rate\Plugin\Block;

use Drupal\Core\Block\BlockBase;
use Drupal\Core\Cache\Cache;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\currency_minimal_rate\CurrencyMinimalRateStorageInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Provides list of minimal currencies rates block.
 *
 * @Block(
 *   id = "currency_minimal_rate_list",
 *   admin_label = @Translation("Currency minimal rate list"),
 *   category = @Translation("Currency minimal rate")
 * )
 */
class CurrencyMinimalRateListBlock extends BlockBase implements ContainerFactoryPluginInterface {

  /**
   * Currency minimal rate database storage.
   *
   * @var \Drupal\currency_minimal_rate\CurrencyMinimalRateStorageInterface
   */
  protected $storage;

  /**
   * Constructs a new CurrencyMinimalRateListBlock.
   *
   * @param array $configuration
   *   A configuration array containing information about the plugin instance.
   * @param string $plugin_id
   *   The plugin_id for the plugin instance.
   * @param mixed $plugin_definition
   *   The plugin implementation definition.
   * @param \Drupal\currency_minimal_rate\CurrencyMinimalRateStorageInterface $storage
   *   Currency minimal rate storage.
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, CurrencyMinimalRateStorageInterface $storage) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->storage = $storage;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('currency_minimal_rate.storage'),
    );
  }

  /**
   * {@inheritdoc}
   */
  public function build() {
    $rates = $this->storage->fetchRates();
    $build['content'] = [
      '#theme' => 'item_list',
      '#list_type' => 'ul',
      '#title' => "Minimal Rates",
      '#items' => [],
    ];
    if (empty($rates)) {
      $build['content']['#items'][] = $this->t('Rates are empty');
    }
    else {
      foreach ($rates as $rate) {
        $item = $rate->getCode() . ' : ' . $rate->getFormattedNumber();
        $build['content']['#items'][] = $item;
      }
    }
    return $build;
  }

  /**
   * {@inheritdoc}
   */
  public function getCacheTags() {
    return Cache::mergeTags(parent::getCacheTags(), ['currency_minimal_rate']);
  }

}
