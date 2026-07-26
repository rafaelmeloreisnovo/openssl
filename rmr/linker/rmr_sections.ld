/* Additive GNU ld/lld script. It does not replace the OpenSSL linker script. */
SECTIONS
{
  .rmr.text ALIGN(64) :
  {
    KEEP(*(.rmr.text))
    KEEP(*(.rmr.text.*))
  }
  .rmr.rodata ALIGN(64) :
  {
    KEEP(*(.rmr.rodata))
    KEEP(*(.rmr.rodata.*))
  }
  .rmr.manifest ALIGN(64) :
  {
    __rmr_manifest_start = .;
    KEEP(*(.rmr.manifest))
    __rmr_manifest_end = .;
  }
  .rmr.kat ALIGN(64) :
  {
    KEEP(*(.rmr.kat))
    KEEP(*(.rmr.kat.*))
  }
}
INSERT AFTER .text;
