<?php

  $modifiers[] = "project.project";
//  $modifiers[] = "organization.organization";

  class mod_wiki extends achievoModule
  {
    function project_project_modifier(&$node) { $this->addWiki($node, "", " project"); }
//    function organization_organization_modifier(&$node) { $this->addWiki($node, "Klant "); }

    function addWiki(&$node, $prefix="", $postfix="")
    {
      useattrib("wiki.wikilinkattribute");
      $node->add(new wikiLinkAttribute($prefix, $postfix));
    }
  }

?>
