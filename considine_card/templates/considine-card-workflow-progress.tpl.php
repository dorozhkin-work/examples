<ul class="create-workflow_breadcrumbs">
  <?php if (!empty($items)) : ?>
    <?php foreach ($items as $item) : ?>
      <?php print $item['content']; ?>
    <?php endforeach; ?>
  <?php endif; ?>
</ul>
