const fs = require('fs');
const path = require('path');
const { JSDOM } = require('jsdom');
const html = fs.readFileSync(path.join(__dirname,'..','enigma-u_v262_en.utf8.html'),'utf8');

(async () => {
  const dom = new JSDOM(html, { url: 'https://example.org/e.html', runScripts:'dangerously', resources:'usable', pretendToBeVisual:true });
  const { window } = dom;
  const doc = window.document;
  function wrapForm(form) {
    return new Proxy(form, { get(target, prop, receiver) {
      if (prop in target) return Reflect.get(target, prop, receiver);
      const named = target.elements.namedItem(prop);
      return named === null ? undefined : named;
    }});
  }
  for (const f of ['a','f','s','k']) doc[f] = wrapForm(doc.forms[f]);
  window.neu(doc.a.preset.value);

  // Classic test vector: Walzenlage "I II III" (left to right), UKW B, ring AAA, start AAA, plaintext AAAAA -> expect BDZGO
  window.model(3); // M3 (gives access to rotors I-VIII + UKW B/C)
  window.wsel(3, 1); // LEFT slot = I
  window.wsel(2, 2); // MID slot = II
  window.wsel(1, 3); // RIGHT slot = III
  window.wsel(0, 2); // UKW B
  window.wlzReset();
  window.rngReset();
  const out = window.enigma('aaaaa');
  console.log('Walzenlage I II III, UKW B, AAA/AAA, "AAAAA" ->', JSON.stringify(out), ' expected: "bdzgo "');
})().catch(e => { console.error(e.stack || e); process.exit(1); });
