#define BOOST_TEST_NO_LIB 1
#include <boost/test/unit_test.hpp>

#include <assert.h>
#include <stdio.h>
#include <iostream>

#include <Inventor/nodes/SoNode.h>

#include <TestSuiteUtils.h>
#include <TestSuiteMisc.h>

using namespace SIM::Coin3D::Coin;

BOOST_AUTO_TEST_SUITE(StandardTests);

BOOST_AUTO_TEST_CASE(loadfiles)
{
  const char * files[] = {
  "models/INVENTOR/57CHEVY.IV",
  "models/INVENTOR/EAN.IV",
  "models/INVENTOR/HEAD.IV",
  "models/INVENTOR/HOUSE.IV",
  "models/VRML1/03PORTTV.WRL",
  "models/VRML1/10CLEAVR.WRL",
  "models/VRML1/11KEY2.WRL",
  "models/VRML1/57CHEVY.WRL",
  "models/VRML1/83_HONDA_ATC.WRL",
  "models/VRML1/9600GXY.WRL",
  "models/VRML1/AAUC.WRL",
  "models/VRML1/AINU1.WRL",
  "models/VRML1/AKDEMO.WRL",
  "models/VRML1/AL.WRL",
  "models/VRML1/AME_OBJECTS.WRL",
  "models/VRML1/ANGFISH.WRL",
  "models/VRML1/ANIHOUSE.WRL",
  "models/VRML1/ANT_DISH.WRL",
  "models/VRML1/APPLE.WRL",
  "models/VRML1/APPLEISH.WRL",
  "models/VRML1/AZTEC-CITY.WRL",
  "models/VRML1/B25.WRL",
  "models/VRML1/BALLOON.WRL",
  "models/VRML1/BANANA.WRL",
  "models/VRML1/BANK.WRL",
  "models/VRML1/BARCELONA.WRL",
  "models/VRML1/BASE.WRL",
  "models/VRML1/BEETHOVEN.WRL",
  "models/VRML1/BEETOVEN.WRL",
  "models/VRML1/BENCH.WRL",
  "models/VRML1/BERTHA.WRL",
  "models/VRML1/BETACAM.WRL",
  "models/VRML1/BICYCLE.WRL",
  "models/VRML1/BIGSPHERE.WRL",
  "models/VRML1/BIPLANE.WRL",
  "models/VRML1/BLACKSUN.WRL",
  "models/VRML1/BLUECREEK.WRL",
  "models/VRML1/BOARD.WRL",
  "models/VRML1/BOOTH.WRL",
  "models/VRML1/BOTTLE1.WRL",
  "models/VRML1/BRIDGE.WRL",
  "models/VRML1/BRUHELE.WRL",
  "models/VRML1/BUG.WRL",
  "models/VRML1/B_ROGERS.WRL",
  "models/VRML1/CABIN.WRL",
  "models/VRML1/CAFFEINE.WRL",
  "models/VRML1/CAMARO.WRL",
  "models/VRML1/CAM_SHAFT.WRL",
  "models/VRML1/CANARD211_TX1.WRL",
  "models/VRML1/CANNON.WRL",
  "models/VRML1/CATHEDRALLO.WRL",
  "models/VRML1/CD.WRL",
  "models/VRML1/CDFDETECTOR.WRL",
  "models/VRML1/CEDAR.WRL",
  "models/VRML1/CHAIR.WRL",
  "models/VRML1/CHECDEM.WRL",
  "models/VRML1/CHESS.WRL",
  "models/VRML1/CHESSBOARD.WRL",
  "models/VRML1/CHICKEN.WRL",
  "models/VRML1/CHOPPER.WRL",
  "models/VRML1/CLOCK.WRL",
  "models/VRML1/CLOCKTWR.WRL",
  "models/VRML1/CLOUD.WRL",
  "models/VRML1/COASTS.WRL",
  "models/VRML1/COFFEMKR.WRL",
  "models/VRML1/COLORFULBISHOP.WRL",
  "models/VRML1/COMPUTER.WRL",
  "models/VRML1/CONIFER.WRL",
  "models/VRML1/COORD3REUSE.WRL",
  "models/VRML1/CORVETTE.WRL",
  "models/VRML1/COVERED_WAGON.WRL",
  "models/VRML1/COW.WRL",
  "models/VRML1/CRAYOLALANDLO.WRL",
  "models/VRML1/CROCODILE.WRL",
  "models/VRML1/CUBE.WRL",
  "models/VRML1/CUBE3.WRL",
  "models/VRML1/CUBEMAC.WRL",
  "models/VRML1/CUBEMAN.WRL",
  "models/VRML1/CUBES.WRL",
  "models/VRML1/CUBOCTAH.WRL",
  "models/VRML1/CURV_4.WRL",
  "models/VRML1/DACTYL.WRL",
  "models/VRML1/DART2.WRL",
  "models/VRML1/DC10.WRL",
  "models/VRML1/DC3.WRL",
  "models/VRML1/DEFFIELD.WRL",
  "models/VRML1/DESTROYR.WRL",
  "models/VRML1/DEVHOUSE.WRL",
  "models/VRML1/DIGRAM.WRL",
  "models/VRML1/DODECAHE.WRL",
  "models/VRML1/DOME.WRL",
  "models/VRML1/DOUBLEVENUS.WRL",
  "models/VRML1/DRUM.WRL",
  "models/VRML1/DSI.WRL",
  "models/VRML1/DUMPTRUCK.WRL",
  "models/VRML1/EARTH.WRL",
  "models/VRML1/EMGAL_EXTERIOR.WRL",
  "models/VRML1/ENGINE.WRL",
  "models/VRML1/ENTCTR.WRL",
  "models/VRML1/ENTREV.WRL",
  "models/VRML1/ETHANOL.WRL",
  "models/VRML1/EXACTO.WRL",
  "models/VRML1/F16.WRL",
  "models/VRML1/FACE.WRL",
  "models/VRML1/FAXMACHN.WRL",
  "models/VRML1/FINAL_ONE_B.WRL",
  "models/VRML1/FLOWERS.WRL",
  "models/VRML1/FORK.WRL",
  "models/VRML1/FORKLIFT.WRL",
  "models/VRML1/FRAME.WRL",
  "models/VRML1/FRAUENKIRCHE.WRL",
  "models/VRML1/FRIDGE.WRL",
  "models/VRML1/FT.WRL",
  "models/VRML1/FULLER.WRL",
  "models/VRML1/FUTRCAR.WRL",
  "models/VRML1/GALCLASS.WRL",
  "models/VRML1/GALILEO.WRL",
  "models/VRML1/GALLEON.WRL",
  "models/VRML1/GALLERY.WRL",
  "models/VRML1/GALVANIC.WRL",
  "models/VRML1/GARD.WRL",
  "models/VRML1/GAZEBO.WRL",
  "models/VRML1/GEARBALL.WRL",
  "models/VRML1/GEARS.WRL",
  "models/VRML1/GENERAL.WRL",
  "models/VRML1/GENOFFCE.WRL",
  "models/VRML1/GERHOW.WRL",
  "models/VRML1/GEW98.WRL",
  "models/VRML1/GIFTBOX.WRL",
  "models/VRML1/GLASS6.WRL",
  "models/VRML1/GLOBE.WRL",
  "models/VRML1/GREENRM1.WRL",
  "models/VRML1/GUIDEBOOK.WRL",
  "models/VRML1/GUITARE.WRL",
  "models/VRML1/GUN.WRL",
  "models/VRML1/HAMMER.WRL",
  "models/VRML1/HAND_DRILL.WRL",
  "models/VRML1/HANOI.WRL",
  "models/VRML1/HEART.WRL",
  "models/VRML1/HOMEGYM.WRL",
  "models/VRML1/HOMEGYM_RED.WRL",
  "models/VRML1/HYDRANT.WRL",
  "models/VRML1/HYDROPLANE.WRL",
  "models/VRML1/ICOSAHED.WRL",
  "models/VRML1/ICOSIDOD.WRL",
  "models/VRML1/IICMINST.WRL",
  "models/VRML1/INDIGO.WRL",
  "models/VRML1/INDY_CAR.WRL",
  "models/VRML1/INGAL_INT_R2V1.WRL",
  "models/VRML1/INGAL_INT_R2V2.WRL",
  "models/VRML1/INGAL_R2V1.WRL",
  "models/VRML1/INGAL_R2V2.WRL",
  "models/VRML1/INTSOL.WRL",
  "models/VRML1/J1.WRL",
  "models/VRML1/JIN01.WRL",
  "models/VRML1/KALIDO_1.WRL",
  "models/VRML1/KARTSWRD.WRL",
  "models/VRML1/KEROLAMP.WRL",
  "models/VRML1/KILLERS/ALLNODE1.WRL",
  "models/VRML1/KILLERS/ARCHES.WRL",
  "models/VRML1/KILLERS/CUBEMAC.WRL",
  "models/VRML1/KILLERS/CUBEPC.WRL",
  "models/VRML1/KILLERS/CUBEUNIX.WRL",
  "models/VRML1/KILLERS/DEFFIELD.WRL",
  "models/VRML1/KILLERS/EMPTY.WRL",
  "models/VRML1/KILLERS/EVIL.WRL",
  "models/VRML1/KILLERS/FACE2.WRL",
  "models/VRML1/KILLERS/FACE3.WRL",
  "models/VRML1/KILLERS/FONTSTRESS.WRL",
  "models/VRML1/KILLERS/LINE.WRL",
  "models/VRML1/KILLERS/MINIMAL.WRL",
  "models/VRML1/KILLERS/MTXMTX.WRL",
  "models/VRML1/KILLERS/MULTSEP.WRL",
  "models/VRML1/KILLERS/NO_EOL.WRL",
  "models/VRML1/KILLERS/ORTHO.WRL",
  "models/VRML1/KILLERS/ORTHO_TS.WRL",
  "models/VRML1/KILLERS/PRECOMP.WRL",
  "models/VRML1/KILLERS/QUEENBIG.WRL",
  "models/VRML1/KILLERS/QUEENBIGNOENUM.WRL",
  "models/VRML1/KILLERS/REPEAT5.WRL",
  "models/VRML1/KILLERS/R_BOX.WRL",
  "models/VRML1/KILLERS/R_BOX_Z.WRL",
  "models/VRML1/KILLERS/SFIMAGE.WRL",
  "models/VRML1/KILLERS/SFIMAGE2.WRL",
  "models/VRML1/KILLERS/SFIMAGE3.WRL",
  "models/VRML1/KILLERS/SPOTLITE.WRL",
  "models/VRML1/KILLERS/SWITCH0.WRL",
  "models/VRML1/KILLERS/T2FACE.WRL",
  "models/VRML1/KILLERS/T2FACE2.WRL",
  "models/VRML1/KILLERS/T2FACE3.WRL",
  "models/VRML1/KILLERS/T2FACE4.WRL",
  "models/VRML1/KILLERS/T2FACE5.WRL",
  "models/VRML1/KILLERS/T2FACE6.WRL",
  "models/VRML1/KILLERS/TEST07.WRL",
  "models/VRML1/KILLERS/TEST12.WRL",
  "models/VRML1/KILLERS/TEST15.WRL",
  "models/VRML1/KILLERS/TEXT02.WRL",
  "models/VRML1/KILLERS/TEXT03.WRL",
  "models/VRML1/KILLERS/TEXTTEST.WRL",
  "models/VRML1/KILLERS/TRI2TXR.WRL",
  "models/VRML1/KILLERS/TXROFF.WRL",
  "models/VRML1/KILLERS/TXROFF19.WRL",
  "models/VRML1/KILLERS/TXROFF_P.WRL",
  "models/VRML1/KINKI_UNI.WRL",
  "models/VRML1/KLINGON.WRL",
  "models/VRML1/KOBE.WRL",
  "models/VRML1/KRAUSE_KAI.WRL",
  "models/VRML1/KRAUSE_KAI_LO.WRL",
  "models/VRML1/KUROSE3.WRL",
  "models/VRML1/L13.WRL",
  "models/VRML1/L19.WRL",
  "models/VRML1/LAMP.WRL",
  "models/VRML1/LAMWITHNAV.WRL",
  "models/VRML1/LASERTRK.WRL",
  "models/VRML1/LEAF.WRL",
  "models/VRML1/LODCUBES.WRL",
  "models/VRML1/LOGONUT_LO.WRL",
  "models/VRML1/LUGER.WRL",
  "models/VRML1/MAN.WRL",
  "models/VRML1/MANILLAFOLDER.WRL",
  "models/VRML1/MEDITBUG.WRL",
  "models/VRML1/MG08.WRL",
  "models/VRML1/MICROSCOPE.WRL",
  "models/VRML1/MIGDAL_SASHA_LO.WRL",
  "models/VRML1/MILLENIUM.WRL",
  "models/VRML1/MOLECULE.WRL",
  "models/VRML1/MOLECULE2.WRL",
  "models/VRML1/MONSTER.WRL",
  "models/VRML1/MOO.WRL",
  "models/VRML1/MOTEUR.WRL",
  "models/VRML1/MTBIKE.WRL",
  "models/VRML1/MUG.WRL",
  "models/VRML1/MULTI.WRL",
  "models/VRML1/MUSHROOM.WRL",
  "models/VRML1/MYHOUSE.WRL",
  "models/VRML1/NAUTILUS.WRL",
  "models/VRML1/NEW.WRL",
  "models/VRML1/NUTMEG.WRL",
  "models/VRML1/NUTRASWEET.WRL",
  "models/VRML1/OCTAHEDR.WRL",
  "models/VRML1/OFFICE.WRL",
  "models/VRML1/OPENBOOK.WRL",
  "models/VRML1/OUT.WRL",
  "models/VRML1/OWEN.WRL",
  "models/VRML1/PALLADIUM.WRL",
  "models/VRML1/PAVILION.WRL",
  "models/VRML1/PEAR.WRL",
  "models/VRML1/PEN.WRL",
  "models/VRML1/PENCIL.WRL",
  "models/VRML1/PENG.WRL",
  "models/VRML1/PHONE.WRL",
  "models/VRML1/PIANO.WRL",
  "models/VRML1/PIANO2.WRL",
  "models/VRML1/PINION.WRL",
  "models/VRML1/PIROGUE.WRL",
  "models/VRML1/PLANT.WRL",
  "models/VRML1/POOL.WRL",
  "models/VRML1/POWERLNS.WRL",
  "models/VRML1/POWER_DRILL.WRL",
  "models/VRML1/PROTEIN.WRL",
  "models/VRML1/PUMP.WRL",
  "models/VRML1/RAGS2.WRL",
  "models/VRML1/RANGE.WRL",
  "models/VRML1/RAT.WRL",
  "models/VRML1/RDHOUSE.WRL",
  "models/VRML1/RECT_ASY.WRL",
  "models/VRML1/REDYELLOW.WRL",
  "models/VRML1/RHOMBICO.WRL",
  "models/VRML1/RHOMBICU.WRL",
  "models/VRML1/RHOMBITR.WRL",
  "models/VRML1/RIBBER.WRL",
  "models/VRML1/RIB_DROP.WRL",
  "models/VRML1/ROT_PIECE.WRL",
  "models/VRML1/ROUND_TABLE.WRL",
  "models/VRML1/ROVER.WRL",
  "models/VRML1/SAILBOAT.WRL",
  "models/VRML1/SAMPLE.WRL",
  "models/VRML1/SEGA3D.WRL",
  "models/VRML1/SGILOGO.WRL",
  "models/VRML1/SGILOGOFLAT.WRL",
  "models/VRML1/SHARK.WRL",
  "models/VRML1/SHUTTLE.WRL",
  "models/VRML1/SIENA.WRL",
  "models/VRML1/SKI.WRL",
  "models/VRML1/SKYSCRPR.WRL",
  "models/VRML1/SLOTMACHINE.WRL",
  "models/VRML1/SNAIL.WRL",
  "models/VRML1/SNUB_CUB.WRL",
  "models/VRML1/SNUB_DOD.WRL",
  "models/VRML1/SOCCER.WRL",
  "models/VRML1/SPATCH_CUBE.WRL",
  "models/VRML1/SPIRAL1.WRL",
  "models/VRML1/SPIRAL2.WRL",
  "models/VRML1/SPIRE.WRL",
  "models/VRML1/STARWARS/E-AT-AT.WRL",
  "models/VRML1/STARWARS/E-AT-ST.WRL",
  "models/VRML1/STARWARS/E-ENDOR.WRL",
  "models/VRML1/STARWARS/E-S-BIKE.WRL",
  "models/VRML1/STARWARS/E-SD-BRG.WRL",
  "models/VRML1/STARWARS/E-SHUTLE.WRL",
  "models/VRML1/STARWARS/E-SSTRDS.WRL",
  "models/VRML1/STARWARS/E-STRDST.WRL",
  "models/VRML1/STARWARS/E-TIE-B.WRL",
  "models/VRML1/STARWARS/E-TIE-DV.WRL",
  "models/VRML1/STARWARS/E-TIE-F.WRL",
  "models/VRML1/STARWARS/E-TIE-I.WRL",
  "models/VRML1/STARWARS/E-TRPRSC.WRL",
  "models/VRML1/STARWARS/E-TRPRST.WRL",
  "models/VRML1/STARWARS/E-VADER.WRL",
  "models/VRML1/STARWARS/O-B-FETT.WRL",
  "models/VRML1/STARWARS/O-BESPIN.WRL",
  "models/VRML1/STARWARS/O-BLASTR.WRL",
  "models/VRML1/STARWARS/O-CARBON.WRL",
  "models/VRML1/STARWARS/O-CLOUDC.WRL",
  "models/VRML1/STARWARS/O-EG-6.WRL",
  "models/VRML1/STARWARS/O-GAFFI.WRL",
  "models/VRML1/STARWARS/O-IG-88.WRL",
  "models/VRML1/STARWARS/O-JAWA-B.WRL",
  "models/VRML1/STARWARS/O-JAWA.WRL",
  "models/VRML1/STARWARS/O-L-SPDR.WRL",
  "models/VRML1/STARWARS/O-PBARGE.WRL",
  "models/VRML1/STARWARS/O-R2-AG4.WRL",
  "models/VRML1/STARWARS/O-SHOPER.WRL",
  "models/VRML1/STARWARS/O-SLAVE.WRL",
  "models/VRML1/STARWARS/R-2-1B.WRL",
  "models/VRML1/STARWARS/R-ANTINF.WRL",
  "models/VRML1/STARWARS/R-C-3PO.WRL",
  "models/VRML1/STARWARS/R-CRUSER.WRL",
  "models/VRML1/STARWARS/R-ESCAPE.WRL",
  "models/VRML1/STARWARS/R-FALCON.WRL",
  "models/VRML1/STARWARS/R-FRIGAT.WRL",
  "models/VRML1/STARWARS/R-HDQRTR.WRL",
  "models/VRML1/STARWARS/R-HNGBAY.WRL",
  "models/VRML1/STARWARS/R-HOTH.WRL",
  "models/VRML1/STARWARS/R-LSABER.WRL",
  "models/VRML1/STARWARS/R-R2-D2.WRL",
  "models/VRML1/STARWARS/R-S-SPDR.WRL",
  "models/VRML1/STARWARS/R-TEMPLE.WRL",
  "models/VRML1/STARWARS/R-TRANSP.WRL",
  "models/VRML1/STARWARS/R-WING-A.WRL",
  "models/VRML1/STARWARS/R-WING-B.WRL",
  "models/VRML1/STARWARS/R-WING-X.WRL",
  "models/VRML1/STARWARS/R-WING-Y.WRL",
  "models/VRML1/STARWARS/R-YODA.WRL",
  "models/VRML1/STEYR.WRL",
  "models/VRML1/STPAULS.WRL",
  "models/VRML1/STREET_LAMP.WRL",
  "models/VRML1/SUBCLASS.WRL",
  "models/VRML1/SUPERSCAPE_BUG.WRL",
  "models/VRML1/SUTTNER.WRL",
  "models/VRML1/T62.WRL",
  "models/VRML1/TABLE.WRL",
  "models/VRML1/TEAPOT.WRL",
  "models/VRML1/TEMPLE.WRL",
  "models/VRML1/TERRAIN_FULL_80K.WRL",
  "models/VRML1/TERRAIN_FULL_9K.WRL",
  "models/VRML1/TEST.WRL",
  "models/VRML1/TETRAHED.WRL",
  "models/VRML1/TEX.WRL",
  "models/VRML1/TEXTURE.WRL",
  "models/VRML1/TEXTURETESS.WRL",
  "models/VRML1/TICTAC.WRL",
  "models/VRML1/TIE.WRL",
  "models/VRML1/TIFFTEX.WRL",
  "models/VRML1/TOP.WRL",
  "models/VRML1/TORUS.WRL",
  "models/VRML1/TPA-LOGO.WRL",
  "models/VRML1/TRACKLIGHTS.WRL",
  "models/VRML1/TRANSMISSION.WRL",
  "models/VRML1/TREE00.WRL",
  "models/VRML1/TREELIGHT.WRL",
  "models/VRML1/TRICERATOPS.WRL",
  "models/VRML1/TRISTEX.WRL",
  "models/VRML1/TRUNCATE.WRL",
  "models/VRML1/TWISTEDCOLUMN.WRL",
  "models/VRML1/UBOAT1.WRL",
  "models/VRML1/UHRTURM.WRL",
  "models/VRML1/VALVE_ASSY_OPEN.WRL",
  "models/VRML1/VASE-JPG.WRL",
  "models/VRML1/VENUS-1.WRL",
  "models/VRML1/VENUS.WRL",
  "models/VRML1/VERTEXMATS.WRL",
  "models/VRML1/VOMITLO.WRL",
  "models/VRML1/VRSOMA.WRL",
  "models/VRML1/VW.WRL",
  "models/VRML1/V_ARCH2A.WRL",
  "models/VRML1/V_PROBE1.WRL",
  "models/VRML1/WALKMAN.WRL",
  "models/VRML1/WARBIRD.WRL",
  "models/VRML1/WHEEL.WRL",
  "models/VRML1/WILCZAK_JOHN_LO.WRL",
  "models/VRML1/WILLOW.WRL",
  "models/VRML1/WINDMILL.WRL",
  "models/VRML1/WINEBOTT.WRL",
  "models/VRML1/WINEBT13.WRL",
  "models/VRML1/X29.WRL",
  "models/VRML1/X29A.WRL",
  "models/VRML1/X_WING.WRL",
  "models/VRML2/01FRIDG1.WRL",
  "models/VRML2/01TABLE4.WRL",
  "models/VRML2/02CHAIR2.WRL",
  "models/VRML2/03ARMOR1.WRL",
  "models/VRML2/03PORTTV.WRL",
  "models/VRML2/04CHAR9.WRL",
  "models/VRML2/04CHCAB4.WRL",
  "models/VRML2/05COFPOT.WRL",
  "models/VRML2/05FORK.WRL",
  "models/VRML2/06CHAR42.WRL",
  "models/VRML2/06ROLTP1.WRL",
  "models/VRML2/07LAMP1.WRL",
  "models/VRML2/07TABL5.WRL",
  "models/VRML2/08NEWSP1.WRL",
  "models/VRML2/08TBLE5.WRL",
  "models/VRML2/09CHAR21.WRL",
  "models/VRML2/09STAIR1.WRL",
  "models/VRML2/09STAIR2.WRL",
  "models/VRML2/10CLEAVR.WRL",
  "models/VRML2/10PHONE3.WRL",
  "models/VRML2/11KEY2.WRL",
  "models/VRML2/11KNIFE.WRL",
  "models/VRML2/12CHAR32.WRL",
  "models/VRML2/12DRESSR.WRL",
  "models/VRML2/AGVRML2.WRL",
  "models/VRML2/ANIMBIKE.WRL",
  "models/VRML2/BM30.WRL",
  "models/VRML2/BRUHELE.WRL",
  "models/VRML2/BRUTE2.WRL",
  "models/VRML2/CARGOSHIP.WRL",
  "models/VRML2/CATALINA.WRL",
  "models/VRML2/CHAMBERV2.WRL",
  "models/VRML2/CLOCKTOWER.WRL",
  "models/VRML2/CONV/57CHEVY.WRL",
  "models/VRML2/CONV/F16.WRL",
  "models/VRML2/CONV/FACE.WRL",
  "models/VRML2/CONV/GALLEON.WRL",
  "models/VRML2/COW_50.WRL",
  "models/VRML2/DININGSET.WRL",
  "models/VRML2/DROS2_1.WRL",
  "models/VRML2/DRUMS_NEP.WRL",
  "models/VRML2/EARTH4.WRL",
  "models/VRML2/EGRET.WRL",
  "models/VRML2/ENTRADA.WRL",
  "models/VRML2/EXAMPLE2.WRL",
  "models/VRML2/EXTRUSION.WRL",
  "models/VRML2/E_FAN.WRL",
  "models/VRML2/FASTSPRUCE.WRL",
  "models/VRML2/FISHTANK.WRL",
  "models/VRML2/FLOWERS.WRL",
  "models/VRML2/GALLERY.WRL",
  "models/VRML2/GALLERY2.WRL",
  "models/VRML2/GALLERYV2.WRL",
  "models/VRML2/GATOR_1.WRL",
  "models/VRML2/GEN_22.WRL",
  "models/VRML2/GOLDGATE.WRL",
  "models/VRML2/GREENWORLD.WRL",
  "models/VRML2/HALLV2.WRL",
  "models/VRML2/HARTLEY.WRL",
  "models/VRML2/IDS_LOGO.WRL",
  "models/VRML2/INTRO_GESAMT.WRL",
  "models/VRML2/JET_ANIM.WRL",
  "models/VRML2/JITTERBUG.WRL",
  "models/VRML2/KILLERS/TEST01.WRL",
  "models/VRML2/KILLERS/TEST02.WRL",
  "models/VRML2/KILLERS/TEST03.WRL",
  "models/VRML2/KILLERS/TEST11.WRL",
  "models/VRML2/KILLERS/TEST16.WRL",
  "models/VRML2/KOTI.WRL",
  "models/VRML2/LABV2.WRL",
  "models/VRML2/LAJOLLA.WRL",
  "models/VRML2/LAVACAVE.WRL",
  "models/VRML2/LIGHTS.WRL",
  "models/VRML2/LISA12.WRL",
  "models/VRML2/LOBSTER.WRL",
  "models/VRML2/MAIN2.WRL",
  "models/VRML2/MARTIAN.WRL",
  "models/VRML2/MENNA.WRL",
  "models/VRML2/MONTEREY.WRL",
  "models/VRML2/MUSEUM.WRL",
  "models/VRML2/NATLIB.WRL",
  "models/VRML2/PAGANSOUND.WRL",
  "models/VRML2/PENGUIN.WRL",
  "models/VRML2/PRE5.WRL",
  "models/VRML2/PSYCHO.WRL",
  "models/VRML2/PSYCHO2.WRL",
  "models/VRML2/PSYCHO3.WRL",
  "models/VRML2/REGAL.WRL",
  "models/VRML2/ROBOTL20.WRL",
  "models/VRML2/RUCUBE.WRL",
  "models/VRML2/SCUBA2.WRL",
  "models/VRML2/SPACEV2.WRL",
  "models/VRML2/SPERM.WRL",
  "models/VRML2/SPINLINE.WRL",
  "models/VRML2/SPINLINE2.WRL",
  "models/VRML2/TEAPOT.WRL",
  "models/VRML2/TERRAIN.WRL",
  "models/VRML2/THE-10.WRL",
  "models/VRML2/TON2.WRL",
  "models/VRML2/TRI4.WRL",
  "models/VRML2/VIBRATRON_VIBRA.WRL",
  "models/VRML2/WIERD.WRL",
  NULL    
  };

  for (int idx = 0; files[idx] != NULL && idx < 0 /* no loading yet */; ++idx) {
    SoNode * fileroot = TestSuite::ReadInventorFile(files[idx]);
    BOOST_CHECK_MESSAGE(fileroot != NULL, "failed to read file..." );
    if (fileroot != NULL) {
      fileroot->ref();
      fileroot->unref();
    }
  }

}

BOOST_AUTO_TEST_SUITE_END();
