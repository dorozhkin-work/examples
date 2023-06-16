<?php

namespace Drupal\currency_minimal_rate\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Url;
use Drupal\currency_minimal_rate\CurrencyMinimalRateComparatorInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\HttpFoundation\RedirectResponse;

/**
 * Class CurrencyMinimalRateController.
 */
class CurrencyMinimalRateController extends ControllerBase {

  /**
   * Currency minimal rate comparator.
   *
   * @var \Drupal\currency_minimal_rate\CurrencyMinimalRateComparatorInterface
   */
  protected $comparator;

  /**
   * CurrencyMinimalRateController constructor.
   *
   * @param \Drupal\currency_minimal_rate\CurrencyMinimalRateComparatorInterface $comparator
   *   Storage.
   */
  public function __construct(CurrencyMinimalRateComparatorInterface $comparator) {
    $this->comparator = $comparator;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('currency_minimal_rate.comparator')
    );
  }

  /**
   * Imports currency minimal rates into database.
   */
  public function import() {

    $this->comparator->execute();

    $providers_names = $this->comparator->getProvidersNames();

    $messenger = $this->messenger();
    $message = $this->t("Minimal rates were imported from such providers: %names", ['%names' => implode(', ', $providers_names)]);
    $messenger->addMessage($message);

    $url = Url::fromRoute('currency_minimal_rate.settings_form')->toString();
    return new RedirectResponse($url);
  }

}
