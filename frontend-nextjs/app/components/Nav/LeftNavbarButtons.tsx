import { Button } from "@/components/ui/button";
import { Hospital, Sparkle, ChevronDown, Dog, Bird, Hop, Wand, Plus, Blocks, Gamepad2 } from "lucide-react";
import {
    DropdownMenuSeparator,
    DropdownMenu,
    DropdownMenuItem,
    DropdownMenuGroup,
    DropdownMenuContent,
    DropdownMenuTrigger,
    DropdownMenuLabel,
} from "@/components/ui/dropdown-menu";
import { usePathname } from "next/navigation";

const ICON_SIZE = 22;

interface LeftNavbarButtonsProps {
    user: IUser | null;
}

export default function LeftNavbarButtons({ user }: LeftNavbarButtonsProps) {
    const isDoctor = user?.user_info.user_type === "doctor";
    const pathname = usePathname();

    let firstWordOfHospital = '';
    if (isDoctor) {
        const hospitalName = (user?.user_info.user_metadata as IDoctorMetadata).hospital_name; 
        firstWordOfHospital = hospitalName ? hospitalName.split(' ')[0] : '';
    }

    const isRoot = pathname === "/";
    const isHome = pathname.includes("/home");

    const shouldShowHospital = isDoctor && firstWordOfHospital.length && isHome;

    return (
        <div className="flex flex-row gap-4 sm:gap-10 items-center">
            <a className="flex flex-row gap-3 items-center" href="/">
                <Wand size={ICON_SIZE} />
                <p
                    className={`flex items-center font-silkscreen text-xl text-stone-800 dark:text-stone-100 relative`}
                >
{shouldShowHospital ? (
                      <>
                        <span>Elato | <span className="text-cyan-700">{firstWordOfHospital}</span></span>
                        <span className="absolute -top-3 -right-3 text-cyan-700"><Plus size={12} strokeWidth={3} /></span>
                      </>
                    ) : (
                      <span>Elato</span>
                    )}
                </p>
            </a>
        </div>
    );
}
