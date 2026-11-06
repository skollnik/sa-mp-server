// New jobs go at the end: the number is what `accounts`.`job` keeps.
enum E_JOB
{
    JOB_NONE,
    JOB_MECHANIC
}

enum E_JOB_INFO
{
    jbName[24],
    jbLevel,
    // The license the job's vehicle needs; LIC_NONE when it drives nothing.
    E_LICENSE:jbLicense,
    jbSkinMale,
    jbSkinFemale,
    E_POINT_TYPE:jbWorkplace
}

new const g_JobInfo[E_JOB][E_JOB_INFO] =
{
    { "Nezaposlen", 0, LIC_NONE, 0,  0,   POINT_CITYHALL },
    { "Mehanicar",  1, LIC_CAR,  50, 192, POINT_WORKSHOP }
};

// The job whose workers may drive the vehicle; JOB_NONE for every other vehicle.
new E_JOB:g_JobVehicle[MAX_VEHICLES];
new bool:g_JobDuty[MAX_PLAYERS];

// A 555 call text: short enough that the line with name and district stays under the 144-character chat limit.
#define MECH_CALL_TEXT          (64)

forward E_JOB:Job_Of(playerid);
forward bool:Job_IsOnDuty(playerid, E_JOB:job);
forward bool:Job_OnDialogResponse(playerid, dialogid, response, listitem);
forward bool:Mech_OnDialogResponse(playerid, dialogid, response, listitem);
