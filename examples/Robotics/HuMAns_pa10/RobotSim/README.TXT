DESCRIPTION DE L'ARBORESCENCE:
-----------------------------
robot = arm2 ou pa10

robotControl.sci = sch�ma de controle du robot (arm2 ou pa10)
                   m�me sch�ma mais diff�rents param�tres
arm2Control.sxd  = sch�ma "equivalent" dataflow en open-office 
robotPlot.sci    = fcts de plot du robot 2D et 3D
robotLaunch.sce  = Lancement de la simulation (�quivalent � un main)
                   on peut lancer sous scilab par 'exec'
                   ou 'scilab -f robotLaunch.sce'


lib/
     modules.sci = Fonctions de base utilis�e pour le controle ind�pendantes
des robots
     simulation.sci = Fonctions de simulation qui appelle l'int�grateur
     plot.sci    = Fonctions de plot des donn�es de simulation

lib/robot (arm2 ou pa10) Mod�le dynamique du robot
 Friction.c, Inertia.c,NLEffects.c, Tags.c, RobotModel.h, Makefile
 => robotmodel.a
 Le programme Maple qui a g�n�r� ces fonctions C est dans MapleCodeGeneration/
 ScilabInterface.c et modele.sci font l'interface entre les fcts C et les fcts Scilab

lib/pa10/pa10Jac.sci  = Modele Geom�trique et jacobienne du PA10
 
-----------------------------------------------------------------------

POUR FAIRE TOURNER L'EXEMPLE PA10:
----------------------------------
+ Lancer Scilab dans RobotSim>

Dans le fichier, pa10Launch.sce, tu peux changer les CI (dur�e, q0), la
loi de commande (en commentant/d�commentant...)

---> exec pa10Launch.sce

-----------------------------------------------------------------------

CE QU'IL ME RESTE A FAIRE:
---------------------------

Par ordre de priorit�:
+ Ecrire la doc
+ Fignoler le code (commentaires, en-t�te)
+ Fignoler arm2 (re-g�n�ration Tags.c, lien Tags.c, faire tourner)
+ Commande cart�sienne arm2




