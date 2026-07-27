/*
 * Additive GNU ld/lld script for the freestanding SHA-256 and uop modules.
 * Only explicit metadata is retained. Code remains in per-function sections so
 * --gc-sections can remove every unreferenced function.
 */
SECTIONS
{
  .rmr.modules ALIGN(64) :
  {
    __rmr_modules_start = .;
    KEEP(*(.rmr.modules))
    __rmr_modules_end = .;
  }

  .rmr.notes ALIGN(16) :
  {
    __rmr_notes_start = .;
    KEEP(*(.rmr.notes))
    __rmr_notes_end = .;
  }

  .rmr.rodata.sha256 ALIGN(64) :
  {
    *(.rmr.rodata.sha256)
  }
}
INSERT AFTER .rodata;
