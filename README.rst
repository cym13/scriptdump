Description
===========

Getting rid of escaped characters in logged teminal sessions isn't as easy as
removing a bunch of them. Scriptdump aims at producing a text output free of
any ansi code or special character to make it easy to edit.

Documentation
=============

::

    Strip a file of its ansi code

    Usage: scriptdump -h
           scriptdump [-f] FILE

    Arguments:
        FILE    The file to be stripped off.
                '-' means standard input

    Options:
        -h, --help   Print this help and exit
        -f, --force  Ignore parsing errors

Bugs
====

The programm has not been thoroughly tested. If you encounter any problem be
sure to report it joining the error message in its entirety and the
typescript used.

If giving the full typescript isn't possible at least join the 16 input bytes
following the position of the error given in the error message.

License
=======

This program is under the GPLv3 License.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

Contact
=======

Main developper: Cédric Picard
Email:           cedric.picard@efrei.net
