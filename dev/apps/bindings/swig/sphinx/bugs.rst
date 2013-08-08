.. _bugs_label:


Known Issues
================================


Exceptions
************************************
- Some exceptions exist in the gpstk.exceptions submodule
  that will never be thrown since Exceptions.i does not
  specify that behavior. They turn into RuntimeExceptions
  at the moment.

