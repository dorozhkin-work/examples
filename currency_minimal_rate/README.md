CONTENTS OF THIS FILE
---------------------
* Introduction
* Requirements
* Installation
* Configuration
* Plugin type

INTRODUCTION
------------

Currency minimal rate module provides block with list of minimal currency rates
based on different currency providers.


REQUIREMENTS
------------
No specific requirements needed.


INSTALLATION
------------
Install the Currency Minimal Rates module as you would normally install
any Drupal contrib module.


CONFIGURATION
--------------
    1. Navigate to Administration > Extend and enable the Currency Minimal Rates
       module.
    2. Navigate to Home > Administration > Configuration > Currency minimal Rates.

PLUGIN TYPE
--------------
Examples of implementation providers inside plugin namespace.
`\Drupal\currency_minimal_rate\Plugin\CurrencyMinimalRateProvider\`

See `\Drupal\currency_minimal_rate\Annotation\CurrencyMinimalRateProvider`
for available properties on CurrencyMinimalRateProvider annotation plugin type.

In most cases it would be enough to make proper annotation type
and implement two functions inside your plugin:
`endpointUrl`
`loadRemoteData`
