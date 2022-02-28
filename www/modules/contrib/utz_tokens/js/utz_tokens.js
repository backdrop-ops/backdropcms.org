/**
 * @file
 * Javascript processing to create tokens to render a date and/or time in the
 * user's time zone.
 */

(function ($) {
  $(document).ready(function() {

    /**
     * Returns a formatted string from a Luxon DateTime object mimicking PHP's
     * DateTime::format() functionality.
     *
     * date    object   Luxon DateTime object
     * format  string  "Y-m-d H:i:s" or similar PHP-style format string
     */
    function formatPHP(date, format) {
      let string = '';
      for (let i of format.match(/(\\)*./g))
      switch (i) {

          // Day

          case 'd': // Day of the month, 2 digits with leading zeros (01 to 31)
            string += date.toFormat('dd');
            break;

          case 'D': // A textual representation of a day, three letters
            string += date.toFormat('ccc');
            break;

          case 'j': // Day of the month without leading zeros (1 to 31)
            string += date.toFormat('d');
            break;

          case 'l': // (lowercase 'L') A full textual representation of the day of the week
            string += date.toFormat('cccc');
            break;

          case 'N': // ISO-8601 representation of the day of the week (1=Monday,...7=Sunday)
            string += date.toFormat('c');
            break;

          case 'S': // English ordinal suffix for the day of the month, 2 characters
            var day = date.toFormat('d');
            if (day == 1) {
              string += 'st';
            }  else if (day == 2) {
              string += 'nd';
            } else if (day == 3) {
              string += 'rd';
            } else {
              string += 'th';
            }
            break;

          case 'w': // Numeric representation of the day of the week (1=Monday,...6=Saturday)
            string += (date.toFormat('c')) % 7;
            break;

          case 'z': // Numeric representation of the day of the year, starting from 0
            string += date.toFormat('o') - 1;
            break;

          // Week

          case 'W': // ISO-860 week number of year, weeks starting on monday
            string += date.toFormat('W');
            break;

          // Month

          case 'F': // A full textual representation of a month, such as January or March
            string += date.toFormat('LLLL');
            break;

          case 'm': // Numeric representation of a month, with leading zeros (01 to 12)
            string += date.toFormat('LL');
            break;

          case 'M': // A short textual representation of a month, three letters (Jan - Dec)
            string += date.toFormat('LLL');
            break;

          case 'n': // Numeric representation of a month, without leading zeros (1 to 12)
            string += date.toFormat('L');
            break;

          case 't': // Number of days in the given month
            string += date.daysInMonth;
            break;

          // Year

          case 'L': // Whether it's a leap year, 1 or 0
            string += +(date.isInLeapYear);
            break;

          case 'o': // ISO-8601 week-numbering year, 4 digits
            string += date.toFormat('kkkk');
            break;

          case 'Y': // A full numeric representation of a year, 4 digits (1999 OR 2003)
            string += date.toFormat('yyyy');
            break;

          case 'y': // A two digit representation of a year (99 OR 03)
            string += date.toFormat('yy');
            break;

          // Time

          case 'a': // Lowercase Ante meridiem and Post meridiem (am or pm)
            string += (date.hour < 12) ? "am" : "pm";
            break;

          case 'A': // Lowercase Ante meridiem and Post meridiem (AM or PM)
            string += (date.hour < 12) ? "AM" : "PM";
            break;

          case 'B': // Swatch internet time
            // Left to be implemented in the future. Does anyone actually use this?
            string += '**TBD**';
            break;

          // Time - Hours

          case 'g': // 12-hour format of an hour without leading zeros (1 to 12)
            string += (date.hour - 1) % 12 + 1;
            break;

          case 'H': // 24-hour format of an hour with leading zeros (00 to 23)
            string += date.hour;
            break;

          case 'h': // 12-hour format of an hour with leading zeros (01 to 12)
            var hour = (date.hour - 1) % 12 + 1;
            if (hour < 10) {
              string += '0';
            }
            string += hour;
            break;

            // Time - Minutes

          case 'i': // Minutes with leading zeros (00 to 59)
            var mi = date.minute;
            string += (mi < 10) ? "0" + mi : mi;
            break;

          // Time - Seconds and fractions thereof

          case 's': // Seconds, with leading zeros (00 to 59)
            var s = date.second;
            string += (s < 10) ? "0" + s : s;
            break;

          case 'u': // Microseconds
            var ms = date.millisecond * 1000;
            ms = ms.toString();
            while (ms.length < 6) {
               ms = '0' + ms;
            }
            string += ms;
            break;

          case 'v': // Milliseconds
            var ms = date.millisecond;
            ms = ms.toString();
            while (ms.length < 3) {
               ms = '0' + ms;
            }
            string += ms;
            break;

          // Timezone

          case 'e': // Timezone identifier
            string += date.toFormat('z');
            break;

          case 'I': // Whether or not the date is in daylight saving time, 1 or 0
            string += +date.isInDST;
            break;

          case 'O': // Different to Greenwich time (GMT) without colon e.g. +0200
            string += date.toFormat('ZZZ');
            break;

          case 'P': // Different to Greenwich time (GMT) with colon e.g. +02:00
            string += date.toFormat('ZZ');
            break;

          // This is a bit weird. PHP 7.3 doesn't seem to recognize 'p' as an
          // option even though it's in the online PHP manual. So we won't either
          // until we figure out what's going on.

  //         case 'p': // Same as P but returns Z instead of +00:00
  //           diff = date.toFormat('ZZ');
  //           if (diff == '+00:00') {
  //             string += 'Z';
  //           } else {
  //             string += diff;
  //           }
  //           break;

          case 'T': // Timezone abbreviation, e.g., EST, MDT, etc.
            string += date.toFormat('ZZZZ');
            break;

          case 'Z': // Timezone offset in seconds.
            string += date.offset * 60;
            break;

          // Full Date/Time

          case 'c': // ISO 8601 date (eg: 2012-11-20T18:05:54.944Z)
            // Note that Luxon's implementation of ISO 8601 is not identical to PHP's.
            string += date.toISO();
            break;

          case 'r': // RFC 2822 formatted date
            string += date.toRFC2822();
            break;

          case 'U': // Seconds since Unix epoch (Jan 1 1970 00:00:00).
            string += date.toSeconds();
            break;

          // Ignore everything else

          default:
            if (i.startsWith("\\")) i = i.substr(1);
            string += i;
        }
      return string;
    };

    // If using browser timezone detection, find any utz-datetime items, then
    // format the string using the PHP format converter.
    var detection = Backdrop.settings.utz_tokens.timezone_detection;
    if (detection == 1 || detection == 2) {
      var selector = 'span.utz-datetime';
      if (detection == 1) {
        selector += '[data-anonymous="1"]';
      }
      jQuery(selector).each(function() {
        var timestamp = +($(this).attr('data-timestamp'));
        var format = $(this).attr('data-format');
        var date = luxon.DateTime.fromSeconds(timestamp);
        var dateStr = formatPHP(date, format);
        $(this).text(dateStr);
        $(this).attr('utz-datetime-processed', '1');
      });
    }
  });
})(jQuery);
