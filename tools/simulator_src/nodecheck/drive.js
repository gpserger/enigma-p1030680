// Proof-of-concept: headlessly drive the Universal Enigma simulator (Daniel Palloks)
// via jsdom, bypassing the GUI, by calling its internal JS functions directly.
const fs = require('fs');
const path = require('path');
const { JSDOM } = require('jsdom');

const htmlPath = path.join(__dirname, '..', 'enigma-u_v262_en.utf8.html');
const html = fs.readFileSync(htmlPath, 'utf8');

(async () => {
  const dom = new JSDOM(html, {
    url: 'https://example.org/enigma.html',
    runScripts: 'dangerously',
    resources: 'usable',
    pretendToBeVisual: true,
  });

  const { window } = dom;
  const doc = window.document;

  // jsdom does not implement two bits of legacy DOM "named access" that real
  // browsers support and that this 2007-era script relies on everywhere:
  //   (1) document.<formName>            -> document.forms.namedItem(formName)
  //   (2) formElement.<controlName>      -> formElement.elements.namedItem(controlName)
  // Patch both in with Proxies so the *unmodified* simulator source runs as-is.
  function wrapForm(form) {
    return new Proxy(form, {
      get(target, prop, receiver) {
        if (prop in target) return Reflect.get(target, prop, receiver);
        const named = target.elements.namedItem(prop);
        return named === null ? undefined : named;
      },
    });
  }
  for (const f of ['a', 'f', 's', 'k']) {
    doc[f] = wrapForm(doc.forms[f]);
  }

  // Now run the page's own bootstrap (normally triggered by <body onload="neu(...)">).
  window.neu(doc.a.preset.value);

  // --- Configure M4 test vector ---
  // U-570 style test: UKW B-thin + Beta, rotors II IV I (left to right: II=left, IV=mid, I=right? we'll
  // instead do the well-known self-consistency check: M3 model, rotors I II III, UKW B, all rings/pos 'A', no plugs.
  // Select "M3" preset (index 3 in the <select name="preset"> menu == model(3))
  window.model(3); // M3
  window.wsel(1, 1); // right rotor = walze I
  window.wsel(2, 2); // mid rotor = walze II
  window.wsel(3, 3); // left rotor = walze III
  window.wsel(0, 2); // UKW = ukw[2] = 'B'
  window.wlzReset(); // positions -> AAAA
  window.rngReset(); // rings -> 01 01 01 01

  const out = window.enigma('aaaaa');
  console.log('M3 I-II-III UKW-B AAAA rings-111 encoding of "AAAAA" ->', JSON.stringify(out));

  // Now M4 config: UKW B-thin + Beta, rotors I, II, IV (right to left slots 1,2,3), all AAAA/rings 1
  window.model(4); // M4
  window.wsel(1, 1); // right = I
  window.wsel(2, 2); // mid = II
  window.wsel(3, 4); // left = IV
  window.wsel(0, 3); // UKW selector value 3 => B-thin + Beta (per wsel() switch-case)
  window.wlzReset();
  window.rngReset();
  const out2 = window.enigma('aaaaa');
  console.log('M4 Beta-I-II-IV UKW-B-thin AAAA rings-1111 encoding of "AAAAA" ->', JSON.stringify(out2));

  console.log('OK: headless driving via jsdom works.');
})().catch((e) => {
  console.error('FAILED:', e && e.stack ? e.stack : e);
  process.exit(1);
});
