<?php
  // Template file for card display/preview/generation.
?>
<div class="card-wrapper">
  <div [considine:texture] class="textureHolder">
    <span [considine:text_1] class="card-text__title">[entityform:field-card-title]</span>
    <span [considine:text_2] class="card-text__content">[entityform:field-card-body]</span>
    <div [considine:text_logo] class="card-logo"></div>
    <div class="card-border"></div>
  </div>
</div>
