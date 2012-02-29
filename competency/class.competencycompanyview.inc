<?php
useattrib ( "atktextattribute" );
useattrib ( "atknumberattribute" );
useattrib ( "atkdateattribute" );
class competencycompanyview extends atkNode {

  function competencycompanyview() {
    $ui = &$this->getUi ();
    $page = $this->getPage ();
    $page->register_script ( moduleDir ( "competency" ) . "javascript/companyview.js" );
    $page->register_scriptcode ( "modDir='" . moduleDir ( "competency" ) . "';" );
    $page->register_loadscript ( "setObservers();" );
    $page->addContent ( $ui->render ( moduleDir ( "competency" ) . "templates/company.tpl", $padrams, "competency" ) );

    $this->atkNode ( "competencycompanyview", NF_NO_ADD | NF_NO_EDIT | NF_NO_DELETE );

    $attr = new atkManyToOneRelation ( "competency_id", "competency.competency", AF_OBLIGATORY | AF_SEARCHABLE );
    $attr->addListColumn ( "domain_id" );

    $attNiv = new atkManyToOneRelation ( "niveau_id", "competency.competencyniveau", AF_OBLIGATORY | AF_SEARCHABLE );
    $attNiv->addListColumn ( "level" );

    $this->add ( $attr, null, 2 );
    $this->add ( $attNiv, null, 3 );

    $attrPers = $this->add ( new atkManyToOneRelation ( "person_id", "employee.employee", AF_OBLIGATORY | AF_SEARCHABLE ) );
    $this->add ( new atkDateAttribute ( "added" ) );

    $this->setTable ( "competency_competences" );
  }

  function descriptor_def() {

    return "[name]";
  }
}
?>