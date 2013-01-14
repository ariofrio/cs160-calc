## Changelog

 - **Scanner implemented**: Jan 13
   - Add `-std=gnu++11` flag to Makefile for better code
   - `next_token()` only fetches another token when `peek_token_available` is
     false, storing the token in `peek_token` and setting
     `peek_token_available` to true.
   - `eat_token()` sets `peek_token_available` to false.
   - `next_token()` fetches another token by reading a character from stdin
     and comparing. For numbers, it reads characters from stdin until too large
     (calling `scan_error()`) or it finds a non-digit, and calling
     `cin.unget()`.
   - `get_line()` returns the member `line_no` which is incremented every time
     a new line character is found.
 - Updated main function per email from baki: Jan 13

TODO: Summarize more commits here.
