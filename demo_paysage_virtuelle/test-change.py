from mido import MidiFile, MidiTrack, Message

mid = MidiFile('rituals.mid')
for i, track in enumerate(mid.tracks):
    for msg in track:
        if msg.type == 'program_change':
            print(f"Ancien instrument dans la piste {i} : {msg.program}")

# Change instrument sur la première piste
for track in mid.tracks:
    for i, msg in enumerate(track):
        if msg.type == 'program_change':
            msg.program = 89  # Flûte
            break  # change le premier seulement

# Si aucun `program_change` n'existe, ajoute-en un :
if all(msg.type != 'program_change' for msg in mid.tracks[0]):
    mid.tracks[0].insert(0, Message('program_change', program=73, time=0))

mid.save('nouveau_fichier.mid')
