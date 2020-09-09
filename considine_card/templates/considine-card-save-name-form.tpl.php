<feildset>
  <div class="inline-row">
    <?php print drupal_render($form['card_name']); ?>
  </div>
</feildset>
<?php if (!empty($form['account'])) : ?>
  <feildset>
    <h4 class="fildset-title"><?php print t('Create an account'); ?></h4>
    <?php print drupal_render($form['account']); ?>
  </feildset>
<?php endif; ?>
<?php if (!empty($form['login'])) : ?>
  <feildset>
    <h4 class="fildset-title"><?php print t('I already have an account'); ?></h4>
    <?php print drupal_render($form['login']); ?>
  </feildset>
<?php endif; ?>
<?php print drupal_render_children($form); ?>
