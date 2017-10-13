
var Module;

if (typeof Module === 'undefined') Module = eval('(function() { try { return Module || {} } catch(e) { return {} } })()');

if (!Module.expectedDataFileDownloads) {
  Module.expectedDataFileDownloads = 0;
  Module.finishedDataFileDownloads = 0;
}
Module.expectedDataFileDownloads++;
(function() {
 var loadPackage = function(metadata) {

    var PACKAGE_PATH;
    if (typeof window === 'object') {
      PACKAGE_PATH = window['encodeURIComponent'](window.location.pathname.toString().substring(0, window.location.pathname.toString().lastIndexOf('/')) + '/');
    } else if (typeof location !== 'undefined') {
      // worker
      PACKAGE_PATH = encodeURIComponent(location.pathname.toString().substring(0, location.pathname.toString().lastIndexOf('/')) + '/');
    } else {
      throw 'using preloaded data can only be done on a web page or in a web worker';
    }
    var PACKAGE_NAME = 'resources.data';
    var REMOTE_PACKAGE_BASE = 'resources.data';
    if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
      Module['locateFile'] = Module['locateFilePackage'];
      Module.printErr('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
    }
    var REMOTE_PACKAGE_NAME = typeof Module['locateFile'] === 'function' ?
                              Module['locateFile'](REMOTE_PACKAGE_BASE) :
                              ((Module['filePackagePrefixURL'] || '') + REMOTE_PACKAGE_BASE);
  
    var REMOTE_PACKAGE_SIZE = metadata.remote_package_size;
    var PACKAGE_UUID = metadata.package_uuid;
  
    function fetchRemotePackage(packageName, packageSize, callback, errback) {
      var xhr = new XMLHttpRequest();
      xhr.open('GET', packageName, true);
      xhr.responseType = 'arraybuffer';
      xhr.onprogress = function(event) {
        var url = packageName;
        var size = packageSize;
        if (event.total) size = event.total;
        if (event.loaded) {
          if (!xhr.addedTotal) {
            xhr.addedTotal = true;
            if (!Module.dataFileDownloads) Module.dataFileDownloads = {};
            Module.dataFileDownloads[url] = {
              loaded: event.loaded,
              total: size
            };
          } else {
            Module.dataFileDownloads[url].loaded = event.loaded;
          }
          var total = 0;
          var loaded = 0;
          var num = 0;
          for (var download in Module.dataFileDownloads) {
          var data = Module.dataFileDownloads[download];
            total += data.total;
            loaded += data.loaded;
            num++;
          }
          total = Math.ceil(total * Module.expectedDataFileDownloads/num);
          if (Module['setStatus']) Module['setStatus']('Downloading data... (' + loaded + '/' + total + ')');
        } else if (!Module.dataFileDownloads) {
          if (Module['setStatus']) Module['setStatus']('Downloading data...');
        }
      };
      xhr.onerror = function(event) {
        throw new Error("NetworkError for: " + packageName);
      }
      xhr.onload = function(event) {
        if (xhr.status == 200 || xhr.status == 304 || xhr.status == 206 || (xhr.status == 0 && xhr.response)) { // file URLs can return 0
          var packageData = xhr.response;
          callback(packageData);
        } else {
          throw new Error(xhr.statusText + " : " + xhr.responseURL);
        }
      };
      xhr.send(null);
    };

    function handleError(error) {
      console.error('package error:', error);
    };
  
      var fetchedCallback = null;
      var fetched = Module['getPreloadedPackage'] ? Module['getPreloadedPackage'](REMOTE_PACKAGE_NAME, REMOTE_PACKAGE_SIZE) : null;

      if (!fetched) fetchRemotePackage(REMOTE_PACKAGE_NAME, REMOTE_PACKAGE_SIZE, function(data) {
        if (fetchedCallback) {
          fetchedCallback(data);
          fetchedCallback = null;
        } else {
          fetched = data;
        }
      }, handleError);
    
  function runWithFS() {

    function assert(check, msg) {
      if (!check) throw msg + new Error().stack;
    }
Module['FS_createPath']('/', 'res', true, true);
Module['FS_createPath']('/', 'fonts', true, true);

    function DataRequest(start, end, crunched, audio) {
      this.start = start;
      this.end = end;
      this.crunched = crunched;
      this.audio = audio;
    }
    DataRequest.prototype = {
      requests: {},
      open: function(mode, name) {
        this.name = name;
        this.requests[name] = this;
        Module['addRunDependency']('fp ' + this.name);
      },
      send: function() {},
      onload: function() {
        var byteArray = this.byteArray.subarray(this.start, this.end);

          this.finish(byteArray);

      },
      finish: function(byteArray) {
        var that = this;

        Module['FS_createDataFile'](this.name, null, byteArray, true, true, true); // canOwn this data in the filesystem, it is a slide into the heap that will never change
        Module['removeRunDependency']('fp ' + that.name);

        this.requests[this.name] = null;
      }
    };

  
    function processPackageData(arrayBuffer) {
      Module.finishedDataFileDownloads++;
      assert(arrayBuffer, 'Loading data file failed.');
      assert(arrayBuffer instanceof ArrayBuffer, 'bad input to processPackageData');
      var byteArray = new Uint8Array(arrayBuffer);
      var curr;
      
          var compressedData = {"data":null,"cachedOffset":696051,"cachedIndexes":[-1,-1],"cachedChunks":[null,null],"offsets":[0,2048,4096,6144,8192,10240,12288,14336,16384,18432,20480,22528,24576,26624,28672,30720,32768,34816,36864,38912,40960,43008,45041,46580,47910,48925,50356,51791,53259,54913,55935,57769,59240,60974,62183,63592,64944,66192,67751,69023,70474,71927,73228,74824,76117,77618,79144,80506,82124,83434,84918,86456,87815,89453,90756,92241,93817,95152,96808,98154,99646,101239,102598,104273,105621,107126,108703,110063,111750,113094,114603,116185,117550,119316,121236,123278,125191,127219,128892,130646,132613,134661,136709,138757,140805,142853,144901,146938,148869,150710,152460,154132,156058,157715,159480,161295,163124,164946,166856,168717,170566,172322,174000,175810,177677,179502,181441,182959,184933,186762,188711,190684,192641,194528,196379,198193,199869,201754,203621,205480,207317,209102,210956,212884,214824,216521,217665,219396,221232,222964,224847,226575,228407,230103,231923,233821,235749,237682,239628,241509,243318,245206,247032,248805,250667,252579,254339,255911,257775,259581,261155,263093,264935,266738,268528,270348,272269,274116,276007,277858,279056,280826,282110,283668,285104,286887,287804,289463,291067,292675,294148,295623,297035,298439,299977,301653,303534,305499,307268,308626,309839,309995,310921,312567,314412,315461,316973,318299,319560,321033,322734,324407,326092,327927,329755,331643,332995,333973,334957,335829,337055,338397,339644,341084,341882,342975,344455,345858,347698,349641,351522,353114,354992,356740,358575,360537,362513,364459,366390,368308,370269,372239,374118,375900,377547,379510,381342,383236,385173,387080,388890,390689,392496,394459,396451,398366,400340,402290,404210,406124,407939,409750,411442,412884,414394,415741,417234,418724,420139,421543,422869,424580,426205,428099,429903,431805,433691,435651,437650,439580,441553,443399,445346,447261,449030,450923,452746,454630,456499,458457,460266,462201,464087,465911,467764,469442,471232,473068,474972,476648,478427,479855,481498,482968,484372,485850,486926,487850,489038,490207,491122,492134,493521,494834,496342,498322,500239,502094,504078,505843,507560,509483,511419,513304,515292,517129,518958,520929,522764,524626,526389,528336,530351,532296,534178,535962,537305,539205,541197,543198,545205,546544,547803,549236,550476,551998,553938,555810,557649,559573,561361,563303,565145,567071,569033,570828,572822,574762,576686,578572,580410,581382,582301,583468,585207,587145,588990,590750,592710,594632,596573,598542,600355,602015,603634,605059,606735,608783,610831,612879,614243,615403,616428,617263,618322,619234,620144,621208,621985,622985,624089,625145,626384,627352,628930,630628,632308,633961,635956,637599,639538,641461,642949,644192,645966,647333,648391,649850,651143,652575,654267,655928,657542,659121,660763,662408,664038,665562,667582,669289,670938,672509,674264,675965,677799,679603,681133,683001,684841,686674,688511,690359,692202,693631,695423],"sizes":[2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2048,2033,1539,1330,1015,1431,1435,1468,1654,1022,1834,1471,1734,1209,1409,1352,1248,1559,1272,1451,1453,1301,1596,1293,1501,1526,1362,1618,1310,1484,1538,1359,1638,1303,1485,1576,1335,1656,1346,1492,1593,1359,1675,1348,1505,1577,1360,1687,1344,1509,1582,1365,1766,1920,2042,1913,2028,1673,1754,1967,2048,2048,2048,2048,2048,2048,2037,1931,1841,1750,1672,1926,1657,1765,1815,1829,1822,1910,1861,1849,1756,1678,1810,1867,1825,1939,1518,1974,1829,1949,1973,1957,1887,1851,1814,1676,1885,1867,1859,1837,1785,1854,1928,1940,1697,1144,1731,1836,1732,1883,1728,1832,1696,1820,1898,1928,1933,1946,1881,1809,1888,1826,1773,1862,1912,1760,1572,1864,1806,1574,1938,1842,1803,1790,1820,1921,1847,1891,1851,1198,1770,1284,1558,1436,1783,917,1659,1604,1608,1473,1475,1412,1404,1538,1676,1881,1965,1769,1358,1213,156,926,1646,1845,1049,1512,1326,1261,1473,1701,1673,1685,1835,1828,1888,1352,978,984,872,1226,1342,1247,1440,798,1093,1480,1403,1840,1943,1881,1592,1878,1748,1835,1962,1976,1946,1931,1918,1961,1970,1879,1782,1647,1963,1832,1894,1937,1907,1810,1799,1807,1963,1992,1915,1974,1950,1920,1914,1815,1811,1692,1442,1510,1347,1493,1490,1415,1404,1326,1711,1625,1894,1804,1902,1886,1960,1999,1930,1973,1846,1947,1915,1769,1893,1823,1884,1869,1958,1809,1935,1886,1824,1853,1678,1790,1836,1904,1676,1779,1428,1643,1470,1404,1478,1076,924,1188,1169,915,1012,1387,1313,1508,1980,1917,1855,1984,1765,1717,1923,1936,1885,1988,1837,1829,1971,1835,1862,1763,1947,2015,1945,1882,1784,1343,1900,1992,2001,2007,1339,1259,1433,1240,1522,1940,1872,1839,1924,1788,1942,1842,1926,1962,1795,1994,1940,1924,1886,1838,972,919,1167,1739,1938,1845,1760,1960,1922,1941,1969,1813,1660,1619,1425,1676,2048,2048,2048,1364,1160,1025,835,1059,912,910,1064,777,1000,1104,1056,1239,968,1578,1698,1680,1653,1995,1643,1939,1923,1488,1243,1774,1367,1058,1459,1293,1432,1692,1661,1614,1579,1642,1645,1630,1524,2020,1707,1649,1571,1755,1701,1834,1804,1530,1868,1840,1833,1837,1848,1843,1429,1792,628],"successes":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]}
;
          compressedData.data = byteArray;
          assert(typeof LZ4 === 'object', 'LZ4 not present - was your app build with  -s LZ4=1  ?');
          LZ4.loadPackage({ 'metadata': metadata, 'compressedData': compressedData });
          Module['removeRunDependency']('datafile_resources.data');
    
    };
    Module['addRunDependency']('datafile_resources.data');
  
    if (!Module.preloadResults) Module.preloadResults = {};
  
      Module.preloadResults[PACKAGE_NAME] = {fromCache: false};
      if (fetched) {
        processPackageData(fetched);
        fetched = null;
      } else {
        fetchedCallback = processPackageData;
      }
    
  }
  if (Module['calledRun']) {
    runWithFS();
  } else {
    if (!Module['preRun']) Module['preRun'] = [];
    Module["preRun"].push(runWithFS); // FS is not initialized yet, wait for it
  }

 }
 loadPackage({"files": [{"audio": 0, "start": 0, "crunched": 0, "end": 37864, "filename": "/HelloWorld.png"}, {"audio": 0, "start": 37864, "crunched": 0, "end": 41460, "filename": "/CloseNormal.png"}, {"audio": 0, "start": 41460, "crunched": 0, "end": 44270, "filename": "/CloseSelected.png"}, {"audio": 0, "start": 44270, "crunched": 0, "end": 44270, "filename": "/res/.gitkeep"}, {"audio": 0, "start": 44270, "crunched": 0, "end": 822822, "filename": "/fonts/arial.ttf"}, {"audio": 0, "start": 822822, "crunched": 0, "end": 848598, "filename": "/fonts/Marker Felt.ttf"}], "remote_package_size": 700147, "package_uuid": "82a19074-7bc4-439c-b3a4-146e50048f50"});

})();
