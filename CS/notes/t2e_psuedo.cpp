bool __usercall C_CSPlayer::TraceToExit@<al>(
    int enterTrace_a1@<edx>, 
    float *vecEnd_a2@<ecx>,
    double broken_double@<st0> /*mac dylib says this is 1.0*/,
    float vecStartPosition.x,
    float vecStartPosition.y, 
    float vecStartPosition.z,
    float vecDir.z_1,
    float vecDir.y_1,
    float vecDir.x_1,
    int exitTrace
)
{
  float vecDir.x; // xmm6_4
  int iFirstContents_standin; // eax
  float vecDir.y; // xmm5_4
  float flDistance_2; // xmm3_4
  float vecDir.z; // xmm4_4
  int exitTrace_1; // esi
  int enterTrace; // ebx
  float *vecEnd; // edi
  float flDistance_3; // xmm3_4
  float vecEnd.x; // xmm3_4
  float vecEnd.y; // xmm0_4
  float vecEnd.z; // xmm1_4
  int iPointContents; // eax
  char exitTrace->startsolid; // dl
  char SURF_NODRAW; // bl
  char IsBreakableEntity_line109; // al
  char IsBreakableEntity_line112; // al
  int world; // eax
  int *enter_entity; // ecx
  char IsBreakableEntity_line90; // al
  float vecNewEnd.x; // [esp+3Ch] [ebp-20h]
  float vecNewEnd.y; // [esp+40h] [ebp-1Ch]
  float vecNewEnd.z; // [esp+44h] [ebp-18h]
  float flDistance; // [esp+48h] [ebp-14h]
  int enterTrace_1; // [esp+4Ch] [ebp-10h]
  int iFirstContents; // [esp+50h] [ebp-Ch]

  vecDir.x = vecDir.x_1;
  iFirstContents_standin = 0;
  vecDir.y = vecDir.y_1;
  flDistance_2 = 0.0;
  vecDir.z = vecDir.z_1;
  exitTrace_1 = exitTrace;
  enterTrace = enterTrace_a1;
  enterTrace_1 = enterTrace_a1;
  vecEnd = vecEnd_a2;
  for ( iFirstContents = 0; ; iFirstContents_standin = iFirstContents )
  {
    flDistance_3 = flDistance_2 + 4.0;
    flDistance = flDistance_3;
    vecEnd.x = vecStartPosition.x + (float)(vecDir.z * flDistance_3);
    vecEnd.y = vecStartPosition.y + vecDir.y * flDistance_3;
    vecEnd.z = vecStartPosition.z + vecDir.x * flDistance_3;
    *vecEnd = vecEnd.x;
    vecEnd[1] = vecEnd.y;
    vecEnd[2] = vecEnd.z;
    vecNewEnd.x = vecEnd.x - (float)(vecDir.z * 4.0);
    vecNewEnd.y = vecEnd.y - (float)(vecDir.y * 4.0);
    vecNewEnd.z = vecEnd.z - (float)(vecDir.x * 4.0);
    if ( !iFirstContents_standin )
      iFirstContents = (**(int (__stdcall ***)(float *, signed int, _DWORD))dword_14F8E300)(vecEnd, 0x4600400B, 0);
    iPointContents = (**(int (__stdcall ***)(float *, signed int, _DWORD))dword_14F8E300)(vecEnd, 0x4600400B, 0);
    if ( iPointContents & 0x600400B && (!(iPointContents & 0x40000000) || iFirstContents == iPointContents) )
      goto LABEL_28;
    TraceRay((int)&vecNewEnd.x, (int)vecEnd, 0x4600400B, 0, exitTrace_1);
    if ( ((int (__thiscall *)(void ***))off_10AC5508[13])(&off_10AC5508) )
      sub_10325780(0x4600400B, 0, exitTrace_1, 0xC2700000); //My guess is this is something to do with DrawDebugLine or something. It's only run when an offset is corrent and stuff.
    if ( *(_BYTE *)(exitTrace_1->startsolid) == 1 && *(_WORD *)(exitTrace_1->surface.flags & 8000))
    {
      TraceLine(&vecStartPosition.x, vecEnd, 0x600400B, *(_DWORD *)(exitTrace->m_pEnt), 0, exitTrace_1);// exitTrace_1 + 76 = m_pEnt
      if ( DidHit(exitTrace_1) && !*(_BYTE *)(exitTrace->startsolid) )// exitTrace_1 + 55 = startsolid
        goto LABEL_13;
LABEL_28:
      vecDir.z = vecDir.z_1;
      vecDir.y = vecDir.y_1;
      vecDir.x = vecDir.x_1;
      goto LABEL_29;
    }
    if ( !DidHit(exitTrace_1) || exitTrace->startsolid )
    {
      if ( *(_DWORD *)(enterTrace->m_pEnt) )
      {
        world = dword_14A8D04C[0] ? (*(int (**)(void))(*(_DWORD *)dword_14A8D04C[0] + 28))() : 0;
        enter_entity = *(int **)(enterTrace->m_pEnt);
        if ( DidHitNonWorldEntity(enter_entity) )     // enter_entity != (int *)world
        {
          if ( IsBreakableEntity(enter_entity, broken_double_1.0) )
          {
            CopyTrace((_DWORD *)exitTrace_1, enterTrace);  // Basically exitTrace = enterTrace

            *(float *)(exitTrace.endpos.x) = vecStartPosition.x + vecDir.z_1;
            *(float *)(exitTrace.endpos.y) = vecStartPosition.y + vecDir.y_1;
            *(float *)(exitTrace.endpos.z) = vecStartPosition.z + vecDir.x_1;
            return true;
          }
        }
      }
      goto LABEL_28;
    }
    SURF_NODRAW = *(_BYTE *)(enterTrace->surface.flags) >> 7;
    if ( *(_BYTE *)(exitTrace_1->surface.flags & 8000) )
    {
      if ( IsBreakableEntity(*(int **)(exitTrace_1->m_pEnt), broken_double_1.0) )
      {
        if ( IsBreakableEntity(*(int **)(enterTrace_1->m_pEnt), broken_double_1.0) )
        {
LABEL_13:
          *vecEnd = *(float *)(exitTrace.endpos.x);
          vecEnd[1] = *(float *)(exitTrace.endpos.y);
          vecEnd[2] = *(float *)(exitTrace.endpos.z);
          return true;
        }
      }
      if ( !SURF_NODRAW )
        goto LABEL_28;
    }
    vecDir.y = vecDir.y_1;
    vecDir.z = vecDir.z_1;
    vecDir.x = vecDir.x_1;
    if (exitTrace->plane.normal.Dot(vecDir) <= 1.f)
    {
      flMultAmount = *(float *)(exit_trace->fraction) * 4.0;
      *vecEnd -= (float)(vecDir.z_1 * flMultAmount);
      vecEnd[1] -= (float)(vecDir.y_1 * flMultAmount);
      vecEnd[2] -=  (float)(vecDir.z_1 * flMultAmount);
      return true;
    }
LABEL_29:                                       // breaking out of the loop to a continue
    flDistance_2 = flDistance;
    if ( flDistance > 90.0 )
      break;
    enterTrace = enterTrace_1;
  }
  return false;
}