const fs = require('fs');
const path = require('path');
const { JSDOM } = require('jsdom');
const html = fs.readFileSync(path.join(__dirname,'..','enigma-u_v262_en.utf8.html'),'utf8');

(async () => {
  const dom = new JSDOM(html, { url: 'https://example.org/e.html', runScripts:'dangerously', resources:'usable', pretendToBeVisual:true });
  const { window } = dom;
  const doc = window.document;
  function wrapForm(form) {
    return new Proxy(form, { get(t,p,r){ if (p in t) return Reflect.get(t,p,r);
      const n = t.elements.namedItem(p); return n===null?undefined:n; }});
  }
  for (const f of ['a','f','s','k']) doc[f] = wrapForm(doc.forms[f]);
  window.neu(doc.a.preset.value);

  // M4: reflector B-thin + Beta, rotors Beta II IV I, rings AAAV, start VJNA,
  // plugboard AT BL DF GJ HM NW OP QY RZ VX -- the real 2006 U-264 break.
  window.model(4);
  window.wsel(0, 3);    // UKW menu value 3 = B-thin + Beta
  window.wsel(3, 2);    // left slot  = rotor II
  window.wsel(2, 4);    // mid slot   = rotor IV
  window.wsel(1, 1);    // right slot = rotor I

  window.switchRngSettings(true);
  window.setw(1, window.setr('22')); // right ring -> V
  window.switchRngSettings(false);

  window.setw(0, window.set('V'));
  window.setw(3, window.set('J'));
  window.setw(2, window.set('N'));
  window.setw(1, window.set('A'));

  const pairs = ['AT','BL','DF','GJ','HM','NW','OP','QY','RZ','VX'];
  const stf = doc.getElementsByName('stf');
  pairs.forEach((p, i) => { stf[i].value = p; });
  window.steck();

  // Ciphertext copied verbatim (lowercased) from py-enigma's KriegsmarineTestCase
  // / tools/verify_py_enigma_m4.py, after stripping indicator groups.
  const ct = 'nczwvusxpnyminhzxmqxsfwxwlkjahshnmcoccakuqpmkcsmhkseinjusblkiosxckubhmllxcsjusrrdvkohulxwccbgvliyxeoahxrhkkfvdrewezlxobafgyujqukgrtvukameurbveksuhhvoyhabcjwmaklfklmyfvnrizrvvrtkofdanjmolbgffleoprgtflvrhowopbekvwmuqfmpwparmfhagkxiibg';
  const out = window.enigma(ct);

  const expected = 'VONVONJLOOKSJHFFTTTEINSEINSDREIZWOYYQNNSNEUNINHALTXXBEIANGRIFFUNTERWASSERGEDRUECKTYWABOSXLETZTERGEGNERSTANDNULACHTDREINULUHRMARQUANTONJOTANEUNACHTSEYHSDREIYZWOZWONULGRADYACHTSMYSTOSSENACHXEKNSVIERMBFAELLTYNNNNNNOOOVIERYSICHTEINSNULL'.toLowerCase();
  const got = out.replace(/ /g, '');
  console.log('got     :', got);
  console.log('expected:', expected);
  console.log('MATCH:', got === expected);
})().catch(e => { console.error(e.stack || e); process.exit(1); });
