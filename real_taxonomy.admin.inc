<?php

/**
 * Module settings page
 */
function real_taxonomy_admin_settings() {
  
  $vocabularies = array(
    0 => t('- Not selected -'),
  );

  foreach (taxonomy_get_vocabularies() as $key => $value) {
    $vocabularies[$value->vid] = $value->name;
  }
	
  $form['real_taxonomy_vocabularies'] = array(
    '#type' => 'select',
    '#title' => t('Vocabulary name'),
    '#options' => $vocabularies,
    '#description' => t('Select the vocabulary to be used by this module'),
    '#default_value' => variable_get('real_taxonomy_vocabularies', 0),
  );

  $form['real_taxonomy_delete_unused_terms'] = array(
    '#type' => 'checkbox',
    '#title' => t('Delete unused terms'),
    '#default_value' => variable_get('real_taxonomy_delete_unused_terms', true),
    '#description' => t('Delete unused terms in vocabulary on node save/update/delete'),
  );

  $form['reset-wrapper'] = array(
    '#type' => 'fieldset',
    '#title' => t('Reset module'),
    '#description' => t('Reset module to initial state and delete all terms from vocabulary'),
    '#collapsible' => true,
    '#collapsed' => true,
  );
  $form['reset-wrapper']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
  );
  $form['#submit'][] = 'real_taxonomy_admin_settings_submit';

  return system_settings_form($form);
}

/**
 * Submit callback.
 */
function real_taxonomy_admin_settings_submit($form, $form_state){
  if($form_state['clicked_button']['#value'] == t('Reset')){
    drupal_goto('real-taxonomy-reset');
  }
}

/**
 * Reset all module data and delete terms from vocabulary
 */
function real_taxonomy_reset(){
  $vid = variable_get('real_taxonomy_vocabularies', 0);
  $vocabularies = taxonomy_get_vocabularies();
  $cancel_path = 'admin/settings/real-taxonomy';
  if($vid && isset($vocabularies[$vid])) {
    $form['#vid'] = $vid;
    $message = t('Are you sure you want to reset module RealTaxonomy?');
    $description = t('All terms from vocabulary %name will be delete!',array('%name' => $vocabularies[$vid]->name));
    return confirm_form($form, $message, $cancel_path, $description, t('Reset'));
  }else{
    drupal_goto($cancel_path);
  }
}

/**
 * Submit callback.
 */
function real_taxonomy_reset_submit($form, &$form_state){
  $vid = $form['#vid'];
  $result = db_query('SELECT tid FROM {term_data} WHERE vid = %d', $vid);
  while ($term = db_fetch_object($result)) {
    taxonomy_del_term($term->tid);
  }
  real_taxonomy_taxonomy('delete', 'vocabulary', array('vid' => $vid));
  cache_clear_all();
  drupal_set_message('All done');
  drupal_goto('admin/settings/real-taxonomy');
}
