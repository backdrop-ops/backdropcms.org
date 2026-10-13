<?php

/**
 * @file
 * This is an implementation of the Porter 2 Stemming algorithm.
 *
 * From http://snowball.tartarus.org/algorithms/english/stemmer.html
 * Adapted by Jennifer Hodgdon of Poplar ProductivityWare, www.poplarware.com.
 */

/**
 * Regular expression defining a vowel for Porter Stemmer.
 */
define('PORTERSTEMMER_VOWEL', '[aeiouy]');

/**
 * Regular expression defining not-a-vowel for Porter Stemmer.
 */
define('PORTERSTEMMER_NOT_VOWEL', '[^aeiouy]');

/**
 * Regular expression defining not a vowel, w, x, or Y for Porter Stemmer.
 */
define('PORTERSTEMMER_NOT_VOWEL_WXY', '[^aeiouywxY]');

/**
 * Regular expression defining a double consonant for Porter Stemmer.
 */
define('PORTERSTEMMER_DOUBLE', '(bb|dd|ff|gg|mm|nn|pp|rr|tt)');

/**
 * Regular expression defining an li-ending for Porter Stemmer purposes.
 */
define('PORTERSTEMMER_LI_END', '[cdeghkmnrt]');

/**
 * Stems a word, using the Porter Stemmer 2 algorithm.
 *
 * @param string $word
 *   Word to stem.
 *
 * @return string
 *   Stemmed word
 */
function porterstemmer_stem($word) {
  // Each of these helper functions returns TRUE if it is time to stop
  // stemming and return. If everything is fine, they modify params by
  // reference, as necessary, for the next function.
  // Position of R1 region in original word.
  $r1 = 0;
  // Position of R1 region in original word.
  $r2 = 0;
  porterstemmer_prestemming($word, $r1, $r2) or
    porterstemmer_exception1($word) or
    porterstemmer_step0($word) or
    porterstemmer_step1a($word) or
    porterstemmer_exception2($word) or
    porterstemmer_step1b($word, $r1) or
    porterstemmer_step1c($word) or
    porterstemmer_step2($word, $r1) or
    porterstemmer_step3($word, $r1, $r2) or
    porterstemmer_step4($word, $r2) or
    porterstemmer_step5($word, $r1, $r2);

  porterstemmer_poststemming($word);
  return $word;
}

/**
 * Returns TRUE if word is too short to continue stemming.
 */
function porterstemmer_too_short($word, $reset = FALSE) {
  static $min_chars = 0;
  if (!$min_chars || $reset) {
    // Get Search module's idea of minimum characters.
    $min_chars = intval(variable_get('minimum_word_size', 3));
    // Porter algorithm cannot handle less than 2 characters.
    if ($min_chars < 2) {
      $min_chars = 2;
    }
  }

  if (drupal_strlen($word) < $min_chars) {
    return TRUE;
  }

  return FALSE;
}

/**
 * Replaces word and calculates return value for steps.
 *
 * If $tmp is long enough, replaces $word with $tmp and returns FALSE
 * to continue stemming process. If $tmp is too short, no replacement
 * and returns TRUE to end stemming process.
 */
function porterstemmer_step_ending(&$word, $tmp) {
  if (porterstemmer_too_short($tmp)) {
    return TRUE;
  }

  $word = $tmp;
  return FALSE;
}

/**
 * Replaces one word ending with another, if tests pass.
 *
 * The return value is TRUE of the ending is present at the end
 * of the word, and FALSE if the ending is not present. The found
 * word ending is also replaced with the given replacement, only if
 * the additional regular expression (if present) matches and if the
 * word is at least the given length.
 *
 * @param string $word
 *    Word to performm search/replace on.
 * @param string $oldend
 *    Ending to check for.
 * @param string $newend
 *    Replacement ending.
 * @param bool $didit
 *    Set to TRUE in the case that a replacement is done; left alone
 *    otherwise.
 * @param string $other
 *    Extra regular expression; must match to allow ending replacement.
 * @param int $minlen
 *    Minimum word length required to allow ending replacement. For
 *    instance, to see if a particular ending is in the R1 region,
 *    pass in $r1 + length of ending as the minimum word length.
 *
 * @return bool
 *    TRUE if ending was at the end of the word, FALSE if not.
 */
function porterstemmer_suffix(&$word, $oldend, $newend, &$didit, $other = NULL, $minlen = 1) {
  // Check to see if the ending is there.
  $end_regexp = '/' . $oldend . '$/';
  if (!preg_match($end_regexp, $word)) {
    // Ending isn't even there.
    return FALSE;
  }

  // Does word match other regular expression?
  if ($other && !preg_match($other, $word)) {
    // No match, so just return without replacing.
    return TRUE;
  }

  // Is word long enough?
  if (drupal_strlen($word) < $minlen) {
    // Too short, so just return without replacing.
    return TRUE;
  }

  // Replace word ending.
  $word = preg_replace($end_regexp, $newend, $word);
  $didit = TRUE;
  return TRUE;
}

/**
 * Checks to see if a word is considered "short" in Porter Stemmer 2.
 *
 * A word is "short" if region R1 doesn't exist, and if it ends in a
 * short syllable. A short syllable is consonant, followed by vowel,
 * followed by consonant not w, x, Y; or else vowel starting a word,
 * followed by a non-vowel.
 *
 * @param string $word
 *   Word to check.
 * @param int $r1
 *   Start position of R1 region in word.
 *
 * @return bool
 *   TRUE if the word is short, false if not.
 */
function porterstemmer_short_word($word, $r1) {

  if (drupal_strlen($word) > $r1) {
    // R1 region exists, so this is not a short word.
    return FALSE;
  }

  // Does it end in one type of short syllable?
  if (preg_match('/^' . PORTERSTEMMER_VOWEL . PORTERSTEMMER_NOT_VOWEL . '$/',
      $word)) {
    return TRUE;
  }

  // Does it end in the other type of short syllable?
  if (preg_match('/' . PORTERSTEMMER_NOT_VOWEL . PORTERSTEMMER_VOWEL .
      PORTERSTEMMER_NOT_VOWEL_WXY . '$/', $word)) {
    return TRUE;
  }

  return FALSE;
}

/**
 * Pre-processes a word for the Porter Stemmer 2 algorithm.
 *
 * Checks for too-short words, removes initial apostrophes, sets y to
 * Y (so as not to be considered a vowel) if y is at start of word or
 * after a vowel. Then calculates the position of the R1 and R2
 * regions in the word.
 *
 * @param string $word
 *   Word to stem, modified in place if successful.
 * @param int $r1
 *   Returns the start position of the "R1" region in the word.
 * @param int $r2
 *   Returns the start position of the "R2" region in the word.
 *
 * @return bool
 *   TRUE if it is time to stop stemming, FALSE to continue.
 */
function porterstemmer_prestemming(&$word, &$r1, &$r2) {
  if (porterstemmer_too_short($word)) {
    return TRUE;
  }

  $tmp = $word;

  // Remove initial apostrophe.
  $tmp = preg_replace("/^'/", '', $tmp);
  if (porterstemmer_too_short($tmp)) {
    return TRUE;
  }

  // Make y -> Y if we should treat it as consonant.
  $tmp = preg_replace('/^y/', 'Y', $tmp);
  $before = 'not going to match';
  while ($before != $tmp) {
    // Do this replacement one by one, to avoid unlikely yyyy issues.
    $before = $tmp;
    // Note: do not use count param to preg_replace - added in 5.10!!
    $tmp = preg_replace('/(' . PORTERSTEMMER_VOWEL . ')y/', '$1Y',
      $tmp, 1);
  }

  // This y/Y step should not have changed the word length.
  $word = $tmp;

  // Find R1 and R2. R1 is the region after the first non-vowel
  // following a vowel. R2 is the region after the first non-vowel
  // following a vowel in R1.
  $max = drupal_strlen($word);
  $r1 = $max;
  $r2 = $max;
  $matches = array();
  $rdef = '/^' . PORTERSTEMMER_NOT_VOWEL . '*' .
    PORTERSTEMMER_VOWEL . '+(' . PORTERSTEMMER_NOT_VOWEL . ')/';

  // Exceptions to R1: If word begins with 'gener', 'commun', or 'arsen',
  // R1 is the remainder of the word.
  if (preg_match('/^(gener|commun|arsen)/', $word, $matches)) {
    $r1 = drupal_strlen($matches[1]);
  }
  elseif (preg_match($rdef, $word, $matches, PREG_OFFSET_CAPTURE)) {
    $r1 = $matches[1][1] + 1;
  };
  $R1 = drupal_substr($word, $r1);
  if ($R1 && preg_match($rdef, $R1, $matches, PREG_OFFSET_CAPTURE)) {
    $r2 = $r1 + $matches[1][1] + 1;
  };

  return FALSE;
}

/**
 * Turn Y back into y to undo pre-processing.
 */
function porterstemmer_poststemming(&$word) {
  $word = str_replace('Y', 'y', $word);
}

/**
 * Step 0 of the algorithm: remove possessive endings.
 *
 * @param string $word
 *   Word to stem, modified in place if successful.
 *
 * @return bool
 *   TRUE if it is time to stop stemming, FALSE to continue.
 */
function porterstemmer_step0(&$word) {
  $tmp = $word;
  $didit = FALSE;
  porterstemmer_suffix($tmp, "'s'", '', $didit) or
  porterstemmer_suffix($tmp, "'s", '', $didit) or
  porterstemmer_suffix($tmp, "'", '', $didit);

  return porterstemmer_step_ending($word, $tmp);
}

/**
 * Step 1a of algorithm: plurals, etc.
 *
 * @param string $word
 *   Word to stem, modified in place if successful.
 *
 * @return bool
 *   TRUE if it is time to stop stemming, FALSE to continue.
 */
function porterstemmer_step1a(&$word) {
  $tmp = $word;
  $didit = FALSE;

  $done = porterstemmer_suffix($tmp, 'sses', 'ss', $didit);

  // The ies/ied endings -- have different replacements depending on
  // if there is more than one letter preceeding. So make sure to
  // test/replace for both conditions.
  if (!$done && porterstemmer_suffix($tmp, 'ies', 'ie', $didit, '/^.?ies$/')) {
    if (!$didit) {
      porterstemmer_suffix($tmp, 'ies', 'i', $didit);
    }
    $done = TRUE;
  }

  if (!$done && porterstemmer_suffix($tmp, 'ied', 'ie', $didit, '/^.?ied$/')) {
    if (!$didit) {
      porterstemmer_suffix($tmp, 'ied', 'i', $didit);
    }
    $done = TRUE;
  }

  if (!$done) {
    porterstemmer_suffix($tmp, 'ss', 'ss', $didit) or
    porterstemmer_suffix($tmp, 'us', 'us', $didit) or
    // Only delete s at end of word if there is at least one vowel that
    // is not immediately before the s.
    porterstemmer_suffix($tmp, 's', '', $didit,
      '/' . PORTERSTEMMER_VOWEL . '.+s$/');
  }

  return porterstemmer_step_ending($word, $tmp);
}

/**
 * Step 1b of algorithm: eed, eedly, ed, edly, ing, ingly.
 *
 * @param string $word
 *   Word to stem, modified in place if successful.
 * @param string $r1
 *   Position of start of R1 region in word.
 *
 * @return bool
 *   TRUE if it is time to stop stemming, FALSE to continue.
 */
function porterstemmer_step1b(&$word, $r1) {
  $tmp = $word;
  $didit = FALSE;

  // Replace these endings if in R1 region.
  $done = (porterstemmer_suffix($tmp, 'eedly', 'ee', $didit, NULL, $r1 + 5) or
    porterstemmer_suffix($tmp, 'eed', 'ee', $didit, NULL, $r1 + 3));

  // Delete these endings if there's a vowel before the ending.
  $didit = FALSE;
  if (!$done) {
    porterstemmer_suffix($tmp, 'edly', '', $didit,
      '/' . PORTERSTEMMER_VOWEL . '.*edly$/') or
      porterstemmer_suffix($tmp, 'ed', '', $didit,
        '/' . PORTERSTEMMER_VOWEL . '.*ed$/') or
      porterstemmer_suffix($tmp, 'ingly', '', $didit,
        '/' . PORTERSTEMMER_VOWEL . '.*ingly$/') or
      porterstemmer_suffix($tmp, 'ing', '', $didit,
        '/' . PORTERSTEMMER_VOWEL . '.*ing$/');
  }

  // If we did one of these replacements, post-process...
  if ($didit) {
    $done = porterstemmer_suffix($tmp, 'at', 'ate', $didit) or
      porterstemmer_suffix($tmp, 'bl', 'ble', $didit) or
      porterstemmer_suffix($tmp, 'iz', 'ize', $didit);
    if (!$done &&
      preg_match('/' . PORTERSTEMMER_DOUBLE . '$/', $tmp)) {
      // Drop last letter if it's a double-letter ending.
      $tmp = drupal_substr($tmp, 0, -1);
      $done = TRUE;
    }
    if (!$done && porterstemmer_short_word($tmp, $r1)) {
      $tmp = $tmp . 'e';
    }
  }

  return porterstemmer_step_ending($word, $tmp);
}

/**
 * Step 1c of algorithm: y suffixes.
 *
 * @param string $word
 *   Word to stem, modified in place if successful.
 *
 * @return bool
 *   TRUE if it is time to stop stemming, FALSE to continue.
 */
function porterstemmer_step1c(&$word) {
  $tmp = $word;
  $didit = FALSE;

  // Replace y or Y by i if the letter before is not a vowel,
  // and that non-vowel is not the beginning of the word.
  $ytest = '/.' . PORTERSTEMMER_NOT_VOWEL . '[Yy]$/';
  porterstemmer_suffix($tmp, 'Y', 'i', $didit, $ytest) or
    porterstemmer_suffix($tmp, 'y', 'i', $didit, $ytest);

  return porterstemmer_step_ending($word, $tmp);
}

/**
 * Step 2 of algorithm: misc endings in region R1.
 *
 * @param string $word
 *   Word to stem, modified in place if successful.
 * @param int $r1
 *   Position of start of R1 region in word.
 *
 * @return bool
 *   TRUE if it is time to stop stemming, FALSE to continue.
 */
function porterstemmer_step2(&$word, $r1) {
  $tmp = $word;
  $didit = FALSE;

  // Search for the longest of these suffixes, and if found in R1, replace.
  porterstemmer_suffix($tmp, 'ational', 'ate', $didit, NULL, $r1 + 7) or
    porterstemmer_suffix($tmp, 'fulness', 'ful', $didit, NULL, $r1 + 7) or
    porterstemmer_suffix($tmp, 'iveness', 'ive', $didit, NULL, $r1 + 7) or
    porterstemmer_suffix($tmp, 'ization', 'ize', $didit, NULL, $r1 + 7) or
    porterstemmer_suffix($tmp, 'ousness', 'ous', $didit, NULL, $r1 + 7) or
    porterstemmer_suffix($tmp, 'biliti', 'ble', $didit, NULL, $r1 + 6) or
    porterstemmer_suffix($tmp, 'lessli', 'less', $didit, NULL, $r1 + 6) or
    porterstemmer_suffix($tmp, 'tional', 'tion', $didit, NULL, $r1 + 6) or
    porterstemmer_suffix($tmp, 'aliti', 'al', $didit, NULL, $r1 + 5) or
    porterstemmer_suffix($tmp, 'ation', 'ate', $didit, NULL, $r1 + 5) or
    porterstemmer_suffix($tmp, 'alism', 'al', $didit, NULL, $r1 + 5) or
    porterstemmer_suffix($tmp, 'entli', 'ent', $didit, NULL, $r1 + 5) or
    porterstemmer_suffix($tmp, 'fulli', 'ful', $didit, NULL, $r1 + 5) or
    porterstemmer_suffix($tmp, 'iviti', 'ive', $didit, NULL, $r1 + 5) or
    porterstemmer_suffix($tmp, 'ousli', 'ous', $didit, NULL, $r1 + 5) or
    porterstemmer_suffix($tmp, 'abli', 'able', $didit, NULL, $r1 + 4) or
    porterstemmer_suffix($tmp, 'alli', 'al', $didit, NULL, $r1 + 4) or
    porterstemmer_suffix($tmp, 'ator', 'ate', $didit, NULL, $r1 + 4) or
    porterstemmer_suffix($tmp, 'anci', 'ance', $didit, NULL, $r1 + 4) or
    porterstemmer_suffix($tmp, 'enci', 'ence', $didit, NULL, $r1 + 4) or
    porterstemmer_suffix($tmp, 'izer', 'ize', $didit, NULL, $r1 + 4) or
    porterstemmer_suffix($tmp, 'bli', 'ble', $didit, NULL, $r1 + 3) or
    // Suffix ogi is only replaced if preceeded by l.
    porterstemmer_suffix($tmp, 'ogi', 'og', $didit,
      '/logi$/', $r1 + 3) or
    // Suffix li is only replaced if preceeded by a valid li-ending.
    porterstemmer_suffix($tmp, 'li', '', $didit,
      '/' . PORTERSTEMMER_LI_END . 'li$/', $r1 + 2);

  return porterstemmer_step_ending($word, $tmp);
}

/**
 * Step 3 of algorithm: misc endings in region R1.
 *
 * @param string $word
 *   Word to stem, modified in place if successful.
 * @param int $r1
 *   Position of start of R1 region in word.
 * @param int $r2
 *   Position of start of R2 region in word.
 *
 * @return bool
 *   TRUE if it is time to stop stemming, FALSE to continue.
 */
function porterstemmer_step3(&$word, $r1, $r2) {
  $tmp = $word;
  $didit = FALSE;

  porterstemmer_suffix($tmp, 'ational', 'ate', $didit, NULL, $r1 + 7) or
    porterstemmer_suffix($tmp, 'tional', 'tion', $didit, NULL, $r1 + 6) or
    porterstemmer_suffix($tmp, 'alize', 'al', $didit, NULL, $r1 + 5) or
    porterstemmer_suffix($tmp, 'ative', '', $didit, NULL, $r2 + 5) or
    porterstemmer_suffix($tmp, 'icate', 'ic', $didit, NULL, $r1 + 5) or
    porterstemmer_suffix($tmp, 'iciti', 'ic', $didit, NULL, $r1 + 5) or
    porterstemmer_suffix($tmp, 'ical', 'ic', $didit, NULL, $r1 + 4) or
    porterstemmer_suffix($tmp, 'ness', '', $didit, NULL, $r1 + 4) or
    porterstemmer_suffix($tmp, 'ful', '', $didit, NULL, $r1 + 3);

  return porterstemmer_step_ending($word, $tmp);
}

/**
 * Step 4 of algorithm: misc endings in region R2.
 *
 * @param string $word
 *   Word to stem, modified in place if successful.
 * @param int $r2
 *   Position of start of R2 region in word.
 *
 * @return bool
 *   TRUE if it is time to stop stemming, FALSE to continue.
 */
function porterstemmer_step4(&$word, $r2) {
  $tmp = $word;
  $didit = FALSE;

  porterstemmer_suffix($tmp, 'ement', '', $didit, NULL, $r2 + 5) or
    porterstemmer_suffix($tmp, 'able', '', $didit, NULL, $r2 + 4) or
    porterstemmer_suffix($tmp, 'ance', '', $didit, NULL, $r2 + 4) or
    porterstemmer_suffix($tmp, 'ence', '', $didit, NULL, $r2 + 4) or
    porterstemmer_suffix($tmp, 'ible', '', $didit, NULL, $r2 + 4) or
    porterstemmer_suffix($tmp, 'ment', '', $didit, NULL, $r2 + 4) or
    porterstemmer_suffix($tmp, 'ant', '', $didit, NULL, $r2 + 3) or
    porterstemmer_suffix($tmp, 'ate', '', $didit, NULL, $r2 + 3) or
    porterstemmer_suffix($tmp, 'ent', '', $didit, NULL, $r2 + 3) or
    porterstemmer_suffix($tmp, 'ion', '', $didit, '/[st]ion$/', $r2 + 3) or
    porterstemmer_suffix($tmp, 'ism', '', $didit, NULL, $r2 + 3) or
    porterstemmer_suffix($tmp, 'iti', '', $didit, NULL, $r2 + 3) or
    porterstemmer_suffix($tmp, 'ive', '', $didit, NULL, $r2 + 3) or
    porterstemmer_suffix($tmp, 'ize', '', $didit, NULL, $r2 + 3) or
    porterstemmer_suffix($tmp, 'ous', '', $didit, NULL, $r2 + 3) or
    porterstemmer_suffix($tmp, 'al', '', $didit, NULL, $r2 + 2) or
    porterstemmer_suffix($tmp, 'er', '', $didit, NULL, $r2 + 2) or
    porterstemmer_suffix($tmp, 'ic', '', $didit, NULL, $r2 + 2);

  return porterstemmer_step_ending($word, $tmp);
}

/**
 * Step 5 of algorithm: e, l endings in region R1/R2.
 *
 * @param string $word
 *   Word to stem, modified in place if successful.
 * @param int $r1
 *   Position of start of R1 region in word.
 * @param int $r2
 *   Position of start of R2 region in word.
 *
 * @return bool
 *   TRUE if it is time to stop stemming, FALSE to continue.
 */
function porterstemmer_step5(&$word, $r1, $r2) {
  $tmp = $word;
  $didit = FALSE;
  $done = FALSE;

  // Delete l at end of word if in R2 and preceded by another l.
  $done = porterstemmer_suffix($tmp, 'll', 'l', $didit, NULL, $r2 + 1);

  // Delete e at end of word if in R2, or in R1 and not preceded by
  // a short syllable.
  $len = drupal_strlen($tmp);
  if (!$done && preg_match('/e$/', $tmp) &&
    ($len > $r2 ||
      ($len > $r1 &&
        !preg_match('/^' . PORTERSTEMMER_VOWEL . PORTERSTEMMER_NOT_VOWEL .
          'e$/', $tmp) &&
        !preg_match('/' . PORTERSTEMMER_NOT_VOWEL . PORTERSTEMMER_VOWEL .
          PORTERSTEMMER_NOT_VOWEL_WXY . 'e$/', $tmp)))) {
    $tmp = drupal_substr($tmp, 0, -1);
  }

  return porterstemmer_step_ending($word, $tmp);
}

/**
 * Checks exceptions for Porter Stemmer.
 *
 * @param string $word
 *   Word to stem, modified in place if successful.
 *
 * @return bool
 *   TRUE if it is time to stop stemming, FALSE to continue.
 */
function porterstemmer_exception1(&$word) {
  // Special cases for stemming. Don't add anything in this list that
  // is shorter than the minimum allowed length!
  $repl = array(
    'skis' => 'ski',
    'skies' => 'sky',
    'dying' => 'die',
    'lying' => 'lie',
    'tying' => 'tie',
    'idly' => 'idl',
    'gently' => 'gentl',
    'ugly' => 'ugli',
    'early' => 'earli',
    'only' => 'onli',
    'singly' => 'singl',
    'sky' => 'sky',
    'news' => 'news',
    'howe' => 'howe',
    'atlas' => 'atlas',
    'cosmos' => 'cosmos',
    'bias' => 'bias',
    'andes' => 'andes',
  );

  // If our word is in that list, we're done.
  if (isset($repl[$word])) {
    $word = $repl[$word];
    return TRUE;
  }

  return FALSE;
}

/**
 * Checks exceptions for Porter Stemmer after Step 1a.
 *
 * @param string $word
 *   Word to stem, modified in place if successful.
 *
 * @return bool
 *   TRUE if it is time to stop stemming, FALSE to continue.
 */
function porterstemmer_exception2(&$word) {
  // The following words are to be left invariant.
  $repl = array(
    'inning' => 1,
    'outing' => 1,
    'canning' => 1,
    'herring' => 1,
    'earring' => 1,
    'proceed' => 1,
    'exceed' => 1,
    'succeed' => 1,
  );

  if (isset($repl[$word])) {
    return TRUE;
  }

  return FALSE;
}
