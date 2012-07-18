About computer security
=======================

Modern operating systems and file systems are very complex.
For most applications, this is a good thing, as it abstracts
away the details of the platform in such a way that programs
perform as well as possible. To make secure software however,
this is slightly problematic.

Deadlock handles sensitive data, and it tries hard to make
this data inaccessible when a vault is not opened.
When a vault is opened however, its contents must be stored
in memory. Data can be kept encrypted in memory, but that
does not significantly increase security; for the data to
be used, it must be decrypted at a certain point, and at that
point, it is stored in memory.

Zeroing memory
--------------
When memory is no longer used, the program gives it back to
the operating system. After a while, the operating system
can give this memory to a different program. By default,
memory is never cleared, so the new program receives ‘garbadge’
that is actually the data from the previous program.
If that data happens to be unencrypted sensitive data,
the other program could easily read the sensitive data.
Fortunately, there is a simple solution to this problem.
Before giving memory back to the operating system,
Deadlock overwrites the data with zeroes, erasing the
unencrypted data from the system.

Swapping and paging
-------------------
For various reasons, the operating system might write memory
to the disk. This is a big problem, because data cannot
be erased from a disk easily. Some filesystems work in
a manner similar to how memory works, and allow the data
to be overwritten with zeroes. However, some filesystems
write changes to different parts of the disk. If sensitive
data happens to be in memory when the operating system decides
to write the memory to the disk, the sensitive data is written
to a permanent storage medium, without an easy way to erase it.
Fortunately, there is a solution to this problem as well.
Both Linux and Windows can be instructed not to swap a memory page
to the disk. Of course this can degrade performance when not enough
memory is available, but for the sake of security, that is not a problem.
Unfortunately, Windows imposes a limit on the amount of memory that
can be prevented from swapping.

Virtualisation
--------------
The above measures work together with the operating system,
to prevent vulnerabilities at the hardware level.
If the operating system runs in a virtual machine, it has no
control over the hardware any more. The progam can instruct
its operating system not to swap memory to a (virtual) disk at best,
but when the host operating system decides to swap the virtual
machine to the disk, those measures do not offer any form of
protection any more. Virtualisation can increase security for
the host operating system, but when sensitive data is handled
in the virtual machine, this imposes a vulnerability.

Best practices
--------------
 1. Do not use Deadlock within a virtual machine,
    as it does not allow sufficient control of the hardware.
 2. Do **not** use hibernate when a vault is open.
	The hibernate functionality writes all memory to the disk
	regardless of whether swapping is prevented or not.
 3. If possible, use full disk encryption, especially for the swap
    partition.
 4. Make sure your system has enough memory available.
 
 A note about open vaults
 ------------------------
 When you open a vault, it is decrypted, and stored in memory.
 All security measures are aimed at making sure that the
 unencrypted data cannot be accessed _after_ the memory
 has been released. When the unencrypted data is in memory,
 it is vulnerable. Of course it is, because if you can use it,
 anyone can. Deadlock only provides secure _storage_ of your data.
 
 To read the unencrypted data from memory, an attacker must have
 access to the system. This could be physical access, but it 
 could be a virus as well. Deadlock is not concerned with these
 problems, because it is the user who should make sure that
 his system is free of viruses and physically secure.