<?php

namespace Drupal\currency_minimal_rate;

use Drupal\Component\Plugin\PluginBase;
use Drupal\Core\Config\ConfigFactory;
use Drupal\Core\Http\ClientFactory;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use GuzzleHttp\Exception\GuzzleException;
use Psr\Log\LoggerInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Base class for currency_minimal_rate_provider plugins.
 */
abstract class CurrencyMinimalRateProviderPluginBase extends PluginBase implements CurrencyMinimalRateProviderInterface, ContainerFactoryPluginInterface {

  /**
   * The HTTP client to fetch the feed data with.
   *
   * @var \Drupal\Core\Http\ClientFactory
   */
  protected $httpClientFactory;

  /**
   * The logger.
   *
   * @var \Psr\Log\LoggerInterface
   */
  protected $logger;

  /**
   * Config factory.
   *
   * @var \Drupal\Core\Config\ConfigFactory
   */
  protected $configFactory;

  /**
   * Debug.
   *
   * @var bool
   */
  protected $debug;

  /**
   * Constructs a new CurrencyMinimalRateProvider object.
   *
   * @param array $configuration
   *   A configuration array containing information about the plugin instance.
   * @param string $plugin_id
   *   The plugin_id for the plugin instance.
   * @param mixed $plugin_definition
   *   The plugin implementation definition.
   * @param \Drupal\Core\Http\ClientFactory $http_client_factory
   *   Drupal http client.
   * @param \Psr\Log\LoggerInterface $logger_channel
   *   Logger.
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, ClientFactory $http_client_factory, LoggerInterface $logger_channel, ConfigFactory $config_factory) {
    parent::__construct($configuration, $plugin_id, $plugin_definition);
    $this->httpClientFactory = $http_client_factory;
    $this->logger = $logger_channel;
    $this->configFactory = $config_factory;
    $this->debug = $this->configFactory->get('currency_minimal_rate.settings')->get('debug');
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('http_client_factory'),
      $container->get('logger.factory')->get('currency_minimal_rate'),
      $container->get('config.factory')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function label() {
    // Cast the label to a string since it is a TranslatableMarkup object.
    return (string) $this->pluginDefinition['label'];
  }

  /**
   * {@inheritdoc}
   */
  public function endpointMethod() {
    return 'GET';
  }

  /**
   * {@inheritdoc}
   */
  public function endpointClient(array $options) {
    $data = [];

    // Prepare for client.
    $client = $this->httpClientFactory->fromOptions();

    try {
      $response = $client->request($this->endpointMethod(), $this->endpointUrl(), $options);

      // Expected result.
      $data = $response->getBody()->getContents();

    }
    catch (GuzzleException $e) {
      $this->logger->error($e->getMessage());
    }

    if ($this->debug) {
      $debug_message = 'Provider "' . $this->label() . '" response:';
      $debug_message .= '<pre><code>' . print_r($data, TRUE) . '</code></pre>';
      $this->logger->debug($debug_message);
    }

    return $data;
  }

  /**
   * Provides currency code mapping list.
   *
   * @return array
   *   List of mapped currency codes.
   */
  protected function getCurrencyCodeMap() {
    return [];
  }

  /**
   * Normalizes currency code.
   *
   * @param string $code
   *   Original currency code.
   *
   * @return string
   *   Normalized currency code.
   */
  public function normalizeCurrencyCode(string $code) {
    $code = mb_strtoupper($code);
    // Getting mapped list to check with.
    $map = $this->getCurrencyCodeMap();
    if (empty($map)) {
      return $code;
    }
    if (isset($map[$code])) {
      $code = $map[$code];
    }
    return $code;
  }

}
