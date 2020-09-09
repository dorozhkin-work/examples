<?php

namespace Drupal\cs_custom\Controller;

use Drupal\Core\Controller\ControllerBase;

/**
 * Defines StyleGuideController class.
 */
class StyleGuideController extends ControllerBase {

  /**
   * Displays the Style Guide page.
   *
   * @return array
   *   Return markup array.
   */
  public function content() {
    $prefix = '
<div>
<div class="container">
<p style="width: 100%;">
<ul class="brand-list">
  <li>
    <div><span class="brand-01"></span><b>brand-01</b><span class="gray">#443465</span></div>
  </li>
  <li>
    <div><span class="brand-02"></span><b>brand-02</b><span class="gray">#7633B9</span></div>
  </li>
  <li>
    <div><span class="brand-03"></span><b>brand-03</b><span class="gray">#A292D9</span></div>
  </li>
  <li>
    <div><span class="brand-04"></span><b>brand-04</b><span class="gray">#D9DFFF</span></div>
  </li>
  <li>
    <div><span class="brand-05"></span><b>brand-05</b><span class="gray">#EEEEFE</span></div>
  </li>
  <li>
    <div><span class="brand-06"></span><b>brand-06</b><span class="gray">#00A8CE</span></div>
  </li>
</ul>
</p>
</div>
<p>
  <div class="light-bg ">
    <div class="container p-40">
    <p><b>Fontawesome duotone icons</b></p>
    <div class="space-between">
    <i class="fad fa-phone-alt"></i>
    <i class="fad fa-user-headset"></i>
    <i class="fad fa-globe"></i>
    <i class="fad fa-envelope"></i>
    <i class="fad fa-map-marker-alt"></i>
    <i class="fad fa-user-circle"></i>
    <i class="fad fa-times-circle"></i>
    <i class="fad fa-life-ring"></i>
    <i class="fad fa-paper-plane"></i>
    <i class="fad fa-times"></i>
    <i class="icon mobile-menu"></i>
    <i class="far fa-angle-left"></i>
    <i class="far fa-angle-right"></i>
    <i class="far fa-angle-down"></i>
    <i class="far fa-angle-up"></i>
    <i class="fad fa-check-circle"></i>
    <i class="icon plus"></i>
    <i class="icon minus"></i>
    </div>
    </div>
  </div>
</p>
<p>
<div class="dark-bg p-40">
<div class="container mr-60">
  <i class="icon feature-1"></i>
  <i class="icon feature-2"></i>
  <i class="icon feature-3"></i>
  <a class="logo navbar-btn" href="/" rel="home" title="Home"><img alt="Home" src="/themes/custom/cloudstudy/images/logo.svg" />
  </a>
</div>
</div>
</p>
<p>
<div class="light-bg p-40">
<div class="container">
   <div class="space-between">
    <a class="btn btn-default"><i class="fad fa-life-ring"></i>Label</a><br />
    <a class="btn btn-default">Label<i class="icon arrow-right"></i></a><br />
    <a class="btn btn-default hover-example">Label<i class="icon arrow-right"></i></a><br />
    <a class="btn btn-primary"><i class="fad fa-paper-plane" aria-hidden="true"></i>Label</a><br />
    <a class="btn btn-primary hover-example"><i class="fad fa-paper-plane" aria-hidden="true"></i>Label</a><br />
    <a class="btn btn-primary">Label</a><br />
    <a>Link</a>
    <a class="hover-example">Link</a>
    </div>
</div>
</div>
</p>
<div class="webform-submission-form">
    ';


    $suffix = '
</div><h1>Header 1</h1>
<h2>Header 2</h2>
<h3>Header 3</h3>
<h4>Header 4</h4>
<h5>Header 5</h5>
<h6>Header 6</h6>

<p><a class="btn btn-default btn-lg">Schedule your demo</a></p>

<p>Phone link<br />
<a class="phone" href="tel:23455442">235547423</a></p>

<p>Email link<br />
<a class="email" href="mailto:info@cloudstudy.eu">info@cloudstudy.eu</a></p>

<p class="address">Address text</p>

<p><strong>Some bold text</strong></p>

<p><em>Some italic text</em></p>

<p>Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.<br />
Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.</p>

<p>Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.</p>

<ul>
	<li>Simple unordered list item 1</li>
	<li>Simple unordered list item 2
	<ul>
		<li>Simple unordered list sub-item 1</li>
		<li>Simple unirdered list sub-item 2</li>
	</ul>
	</li>
	<li>Simple unordered list item 3</li>
</ul>

<ol>
	<li>Simple ordered list item 1</li>
	<li>Simple ordered list item 2</li>
	<li>Simple ordered list item 3</li>
</ol>

<p>List from Pricing page:</p>

<ul class="checked-list">
	<li>Learning management system</li>
	<li>Your internal courses and instructions</li>
</ul>

<p>&nbsp;</p>

<div class="price">
<span class="cost">6</span>
<sup class="currency">€</sup>
<span class="price-description">per user per year</span>
</div>';

    $form = [];

    $form['input_field'] = [
      '#type' => 'textfield',
      '#title' => t('Input'),
      '#attributes' => [
        'placeholder' => t('Placeholder'),
      ],
    ];
    $form['focused_field'] = [
      '#type' => 'textfield',
      '#title' => t('Focus'),
      '#attributes' => [
        'autofocus' => t('autofocus'),
      ],
    ];
    $form['error_field'] = [
      '#type' => 'textfield',
      '#title' => t('Error'),
      '#required' => TRUE,
      '#wrapper_attributes' => [
        'class' => [
          'error',
          'has-error',
        ],
      ],
      '#field_suffix' => '<div class="form-item--error-message alert alert-danger alert-sm">Error message</div>',
    ];
    $form['#prefix'] = $prefix;
    $form['#suffix'] = $suffix;

    \Drupal::messenger()->addMessage('Drupal standard status message.');
    \Drupal::messenger()->addMessage('Drupal standard warning message.', 'warning');
    \Drupal::messenger()->addMessage('Drupal standard error message.', 'error');

    $output = [];
    $output['form'] = $form;
    $output['header-waves'] = [
      '#type' => 'inline_template',
      '#template' => '<p>
<svg id="header-waves" width="1920" height="115" viewBox="0 0 1920 115" fill="none" xmlns="http://www.w3.org/2000/svg" preserveAspectRatio="xMidYMid meet">
  <path class="header-wave-1" d="M0.000694275 72.0332C322.671 129.384 642.671 129.384 960 72.0332C1277.33 14.6816 1597.33 14.6816 1920 72.0332V0.236216H0.000694275V72.0332Z" fill="#443465"/>
  <path class="header-wave-2" d="M459.999 0.656525C274 0.656525 57.5 81.5439 57.5 81.5439C216.55 104.114 305.5 112.176 458.999 115.526C645.417 112.793 746.999 104.996 914.999 80.108C914.999 80.108 645.999 0.656525 459.999 0.656525Z" fill="#802EC0"/>
</svg>
</p>',
      '#context' => []
    ];
    $output['footer-waves'] = [
      '#type' => 'inline_template',
      '#template' => '<p>
<svg id="footer-waves"  width="1920" height="180" viewBox="0 0 1920 180" fill="none" xmlns="http://www.w3.org/2000/svg" preserveAspectRatio="xMidYMid meet">
  <path class="footer-wave-1" d="M0 87.2042C318.222 -17.647 638.222 -27.9461 960 56.3064C1281.78 140.559 1601.78 166.842 1920 135.154V179.439H0V87.2042Z" fill="#7633B9"/>
  <path class="footer-wave-2" d="M1920 132.724C1567.13 34.4033 1216.51 27.3684 868.13 111.62C550.162 179.126 550.162 179.126 550.162 179.126H1920V132.724Z" fill="#A292D9"/>
  <path class="footer-wave-3" d="M1920 134.436C1597.33 74.5231 1277.33 74.5231 960 134.436C642.667 194.348 322.667 194.348 0.00012207 134.436V179.439H1920V134.436Z" fill="#443465"/>
</svg>
</p>',
      '#context' => []
    ];
    return $output;
  }
}
