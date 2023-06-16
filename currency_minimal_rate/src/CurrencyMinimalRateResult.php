<?php

namespace Drupal\currency_minimal_rate;

/**
 * Value object for currency minimal rate.
 */
class CurrencyMinimalRateResult {

  /**
   * Currency rate code.
   *
   * @var string
   */
  protected $code;

  /**
   * Currency rate number.
   *
   * @var string
   */
  protected $number;

  /**
   * Rate decimals.
   *
   * @var int
   */
  protected $decimals;

  /**
   * Constructor.
   */
  public function __construct($code, $number, $decimals = 3) {
    $this->code = (string) $code;
    $this->number = (float) $number;
    $this->decimals = $decimals;
  }

  /**
   * Return rate`s currency code.
   *
   * @return string
   */
  public function getCode() {
    return mb_strtoupper($this->code);
  }

  /**
   * Returns rate`s number.
   *
   * @return string
   */
  public function getNumber() {
    return $this->number;
  }

  /**
   * Returns rate`s formatted number.
   *
   * @return string
   */
  public function getFormattedNumber() {
    return number_format($this->getNumber(), $this->decimals);
  }

}
