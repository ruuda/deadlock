Unicode
=======

Dealdock stores all data in unicode format,
and it uses UTF-8 to store text internally,
as well as for the internal and exported JSON
structure. Most applications can handle
UTF-8, but I recommend not using any non-ASCII
character at all. This avoids normalization
issues, simplifies searching, and avoids
most encoding problems.

UTF-8 on Windows
----------------
The Windows operating system is the only major
operating system that does not use UTF-8 internally.
Windows handles text in UTF-16 format in unicode-aware
applications, and handles text in the local codepage
for non-unicode applications.

It certainly is possible to work with UTF-8 on
Windows, but this means all text has to be converted
to UTF-16 (or the other way around) when interfacing
with the operating system. The most significant problem,
is that it requires all console I/O functions to
have special implementations on Windows. I decided
not to do this, because of all the code duplication.
It is possible to set the console mode to UTF-8
on Windows (which is what Deadlock does),
but it simply ignores non-ASCII characters
and substitutes a question mark, instead of displaying
two weird characters.

To handle non-ASCII characters on Windows,
you must redirect the output of the program to a file.
The following example retrieves your GitHub password
from the vault `vault.dlk` and stores it in `password.txt`.

    > deadlock -g github vault.dlk > password.txt

You will not see the `Passphrase: ` prompt or any error
messages, but you can still type your passphrase,
and the output will be written to the file. **Please
note that this is a major security risk. Your password
is now in plaintext on the disk, form which it cannot
easily be removed.**

If you run MinGW (or a Git Bash,
or something similar which has an unicode version of `cat`),
you can use `cat` to display the UTF-8 characters.

    $ deadlock -g github vault.dlk | cat

Windows does **not** send through the input command-line parameters
correctly, so you cannot correctly pass non-ASCII characters
to the program on Windows. Please note that those characters will
work in some cases, but they will cause malformed UTF-8 text.